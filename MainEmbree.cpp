//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "Camera.h"
#include "Image.h"
#include "MaterialSet.h"
#include "Sampling.h"
#include "Scene.h"
#include "Sphere.h"
#include "Timer.h"
#include "Math\Random.h"

#include "embree3/rtcore.h"


using namespace std;
using namespace concurrency;
using namespace Math;


// Image parameters
constexpr int IMAGE_WIDTH = 1280;
constexpr int IMAGE_HEIGHT = 720;
constexpr int NUM_SAMPLES = 16;
constexpr float INV_SAMPLES = 1.0f / static_cast<float>(NUM_SAMPLES);
constexpr float ASPECT = static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);
constexpr int MAX_RECURSION = 50;
constexpr int TILE_WIDTH = 8;
constexpr int TILE_HEIGHT = 8;

// Scene parameters
constexpr int SPHERE_GRID_SIZE = 11;

// Feature flags
constexpr bool g_threaded = true;
constexpr bool g_streams = false;
constexpr bool g_recursive = false;

// Stats
atomic_size_t s_totalRays = 0;

MaterialSet materialSet;

// Scene
struct EmbreeSphere
{
	shared_ptr<Sphere> sphere;
	RTCGeometry geom;
	uint32_t geomId;
};
vector<EmbreeSphere> spheres;


void AddSphere(shared_ptr<IPrimitive> sphere)
{
	EmbreeSphere eSphere;
	eSphere.sphere = dynamic_pointer_cast<Sphere>(sphere);
	eSphere.geom = nullptr;
	eSphere.geomId = RTC_INVALID_GEOMETRY_ID;
	spheres.push_back(eSphere);
}


void RandomScene()
{
	auto sphere = Sphere::Make(Vector3(0.0f, -1000.0f, 0.0f), 1000.0f);
	materialSet.AddLambertian(Vector3(0.5f, 0.5f, 0.5f));
	AddSphere(sphere);

	for (int a = -SPHERE_GRID_SIZE; a < SPHERE_GRID_SIZE; ++a)
	{
		for (int b = -SPHERE_GRID_SIZE; b < SPHERE_GRID_SIZE; ++b)
		{
			float radius = 0.2f + g_RNG.NextFloat(-0.1f, 0.1f);
			Vector3 center(a + 0.9f * g_RNG.NextFloat(), radius, b + 0.9f * g_RNG.NextFloat());

			if (Length(center - Vector3(4.0f, 0.2f, 0.0f)) > 0.9f)
			{
				sphere = Sphere::Make(center, radius);

				float chooseMat = g_RNG.NextFloat();

				if (chooseMat < 0.8f) // Lambertian
				{
					materialSet.AddLambertian(Vector3(g_RNG.NextFloat() * g_RNG.NextFloat(), g_RNG.NextFloat() * g_RNG.NextFloat(), g_RNG.NextFloat() * g_RNG.NextFloat()));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					materialSet.AddMetallic(Vector3(0.5f * (1.0f + g_RNG.NextFloat()), 0.5f * (1.0f + g_RNG.NextFloat()), 0.5f * (1.0f + g_RNG.NextFloat())), 0.5f * g_RNG.NextFloat());
				}
				else // Dielectric
				{
					materialSet.AddDielectric(1.5f);
				}

				AddSphere(sphere);
			}
		}
	}

	sphere = Sphere::Make(Vector3(0.0f, 1.0f, 0.0f), 1.0f);
	materialSet.AddDielectric(1.5f);
	AddSphere(sphere);

	sphere = Sphere::Make(Vector3(-4.0f, 1.0f, 0.0f), 1.0f);
	materialSet.AddLambertian(Vector3(0.4f, 0.2f, 0.1f));
	AddSphere(sphere);

	sphere = Sphere::Make(Vector3(4.0f, 1.0f, 0.0f), 1.0f);
	materialSet.AddMetallic(Vector3(0.7f, 0.6f, 0.5f), 0.0f);
	AddSphere(sphere);
}


void SphereBoundsFunc(const RTCBoundsFunctionArguments* args)
{
	EmbreeSphere* embSphere = (EmbreeSphere*)args->geometryUserPtr;
	Sphere* sphere = embSphere->sphere.get();

	RTCBounds* bounds = args->bounds_o;
	bounds->lower_x = sphere->GetCenterX() - sphere->GetRadius();
	bounds->lower_y = sphere->GetCenterY() - sphere->GetRadius();
	bounds->lower_z = sphere->GetCenterZ() - sphere->GetRadius();
	bounds->upper_x = sphere->GetCenterX() + sphere->GetRadius();
	bounds->upper_y = sphere->GetCenterY() + sphere->GetRadius();
	bounds->upper_z = sphere->GetCenterZ() + sphere->GetRadius();
}


void SphereIntersectFunc(const RTCIntersectFunctionNArguments* args)
{
	int* valid = args->valid;
	void* ptr = args->geometryUserPtr;
	unsigned int N = args->N;
	RTCRayHitN* rayhit = (RTCRayHitN*)args->rayhit;
	RTCRayN* rays = RTCRayHitN_RayN(rayhit, N);
	RTCHitN* hits = RTCRayHitN_HitN(rayhit, N);
	unsigned int primID = args->primID;

	EmbreeSphere* embSphere = (EmbreeSphere*)args->geometryUserPtr;
	Sphere* sphere = embSphere->sphere.get();

	assert(args->N == 1);

	if (!valid[0]) return;

	const Vector3 org = Vector3(RTCRayN_org_x(rays, N, 0), RTCRayN_org_y(rays, N, 0), RTCRayN_org_z(rays, N, 0));
	const Vector3 dir = Vector3(RTCRayN_dir_x(rays, N, 0), RTCRayN_dir_y(rays, N, 0), RTCRayN_dir_z(rays, N, 0));
	float& ray_tnear = RTCRayN_tnear(rays, N, 0);
	float& ray_tfar = RTCRayN_tfar(rays, N, 0);

	const Vector3 v = org - sphere->GetCenter();
	const float A = Dot(dir, dir);
	const float B = 2.0f * Dot(v, dir);
	const float C = Dot(v, v) - sphere->GetRadiusSq();
	const float D = B * B - 4.0f * A * C;
	if (D < 0.0f) return;
	const float Q = sqrt(D);
	const float rcpA = 1.0f / A;
	const float t0 = 0.5f * rcpA * (-B - Q);
	const float t1 = 0.5f * rcpA * (-B + Q);

	RTCHit potentialHit;
	potentialHit.u = 0.0f;
	potentialHit.v = 0.0f;
	potentialHit.instID[0] = args->context->instID[0];
	potentialHit.geomID = embSphere->geomId;
	potentialHit.primID = primID;

	if ((ray_tnear < t0) & (t0 < ray_tfar))
	{
		const Vector3 Ng = org + t0 * dir - sphere->GetCenter();
		potentialHit.Ng_x = Ng.GetX();
		potentialHit.Ng_y = Ng.GetY();
		potentialHit.Ng_z = Ng.GetZ();

		ray_tfar = t0;
		
		rtcCopyHitToHitN(hits, &potentialHit, N, 0);
	}

	if ((ray_tnear < t1) & (t1 < ray_tfar))
	{
		const Vector3 Ng = org + t1 * dir - sphere->GetCenter();
		potentialHit.Ng_x = Ng.GetX();
		potentialHit.Ng_y = Ng.GetY();
		potentialHit.Ng_z = Ng.GetZ();

		ray_tfar = t1;
	
		rtcCopyHitToHitN(hits, &potentialHit, N, 0);
	}
}


void SphereIntersectFuncN(const RTCIntersectFunctionNArguments* args)
{
	int* valid = (int*)args->valid;
	void* ptr = args->geometryUserPtr;
	unsigned int N = args->N;
	RTCRayHitN* rayhit = (RTCRayHitN*)args->rayhit;
	RTCRayN* rays = RTCRayHitN_RayN(rayhit, N);
	RTCHitN* hits = RTCRayHitN_HitN(rayhit, N);
	unsigned int primID = args->primID;

	/* iterate over all rays in ray packet */
	for (unsigned int ui = 0; ui < N; ui += 1)
	{
		/* calculate loop and execution mask */
		unsigned int vi = ui + 0;
		if (vi >= N) continue;

		/* ignore inactive rays */
		if (valid[vi] != -1) continue;

		const Vector3 ray_org = Vector3(RTCRayN_org_x(rays, N, ui), RTCRayN_org_y(rays, N, ui), RTCRayN_org_z(rays, N, ui));
		const Vector3 ray_dir = Vector3(RTCRayN_dir_x(rays, N, ui), RTCRayN_dir_y(rays, N, ui), RTCRayN_dir_z(rays, N, ui));
		float& ray_tnear = RTCRayN_tnear(rays, N, ui);
		float& ray_tfar = RTCRayN_tfar(rays, N, ui);

		const Sphere* sphere = spheres[primID].sphere.get();
		const Vector3 v = ray_org - sphere->GetCenter();
		const float A = Dot(ray_dir, ray_dir);
		const float B = 2.0f * Dot(v, ray_dir);
		const float C = Dot(v, v) - sphere->GetRadiusSq();
		const float D = B * B - 4.0f * A * C;
		if (D < 0.0f) continue;
		const float Q = sqrt(D);
		const float rcpA = 1.0f / A;
		const float t0 = 0.5f * rcpA * (-B - Q);
		const float t1 = 0.5f * rcpA * (-B + Q);

		RTCRayHit rtc_ray = rtcGetRayHitFromRayHitN(rayhit, N, ui);

		RTCHit potentialhit;
		potentialhit.u = 0.0f;
		potentialhit.v = 0.0f;
		potentialhit.instID[0] = args->context->instID[0];
		potentialhit.geomID = spheres[primID].geomId;
		potentialhit.primID = primID;

		if ((ray_tnear < t0) & (t0 < ray_tfar))
		{
			const Vector3 Ng = ray_org + t0 * ray_dir - sphere->GetCenter();
			potentialhit.Ng_x = Ng.GetX();
			potentialhit.Ng_y = Ng.GetY();
			potentialhit.Ng_z = Ng.GetZ();

			
			ray_tfar = t0;
			rtcCopyHitToHitN(hits, &potentialhit, N, ui);
		}

		if ((ray_tnear < t1) & (t1 < ray_tfar))
		{
			const Vector3 Ng = ray_org + t1 * ray_dir - sphere->GetCenter();
			potentialhit.Ng_x = Ng.GetX();
			potentialhit.Ng_y = Ng.GetY();
			potentialhit.Ng_z = Ng.GetZ();
			
			ray_tfar = t1;
			rtcCopyHitToHitN(hits, &potentialhit, N, ui);
		}
	}
}


void BuildEmbreeScene(RTCDevice embreeDevice, RTCScene embreeScene)
{
	const size_t numPrimitives = spheres.size();

	for (size_t i = 0; i < numPrimitives; ++i)
	{
		RTCGeometry geom = rtcNewGeometry(embreeDevice, RTC_GEOMETRY_TYPE_USER);
		unsigned int geomID = rtcAttachGeometry(embreeScene, geom);

		spheres[i].geom = geom;
		spheres[i].geomId = geomID;

		rtcSetGeometryUserPrimitiveCount(geom, 1);
		rtcSetGeometryUserData(geom, &spheres[i]);
		rtcSetGeometryBoundsFunction(geom, SphereBoundsFunc, nullptr);

		if (g_streams)
		{
			rtcSetGeometryIntersectFunction(geom, SphereIntersectFuncN);
		}
		else
		{
			rtcSetGeometryIntersectFunction(geom, SphereIntersectFunc);
		}

		rtcCommitGeometry(geom);
		rtcReleaseGeometry(geom);
	}

	rtcCommitScene(embreeScene);
}


Vector3 LinearToSRGB(Vector3 linearRGB)
{
	XMVECTOR T = XMVectorSaturate(linearRGB);
	XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(1.0f / 2.4f)), 1.055f), XMVectorReplicate(0.055f));
	result = XMVectorSelect(result, XMVectorScale(T, 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
	return Vector3(XMVectorSelect(T, result, g_XMSelect1110));
}


Vector3 GetColor_Recursive(const Ray& ray, const RTCScene& scene, int depth, uint32_t& state)
{
	Hit hit;
	++s_totalRays;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	RTCRayHit rayHit;
	rayHit.ray.org_x = ray.org.GetX();
	rayHit.ray.org_y = ray.org.GetY();
	rayHit.ray.org_z = ray.org.GetZ();
	rayHit.ray.dir_x = ray.dir.GetX();
	rayHit.ray.dir_y = ray.dir.GetY();
	rayHit.ray.dir_z = ray.dir.GetZ();
	rayHit.ray.tnear = 0.001f;
	rayHit.ray.tfar = FLT_MAX;
	rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

	rtcIntersect1(scene, &context, &rayHit);

	if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Ray scattered;
		Vector3 attenuation;

		hit.pos = ray.Eval(rayHit.ray.tfar);
		hit.t = rayHit.ray.tfar;
		hit.normal = Normalize(Vector3(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z));
		
		if (depth < MAX_RECURSION && materialSet.Scatter(rayHit.hit.geomID, ray, hit, attenuation, scattered, state))
		{
			return attenuation * GetColor_Recursive(scattered, scene, depth + 1, state);
		}
		else
		{
			return Vector3(kZero);
		}
	}

	Vector3 unitDir = Normalize(ray.Direction());
	float t = 0.5f * (unitDir.GetY() + 1.0f);
	return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.5f, 0.7f, 1.0f);
}

Vector3 GetColor_Iterative(Ray ray, const RTCScene& scene, uint32_t& state)
{
	Hit hit;
	++s_totalRays;

	Vector3 color(kOne);

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	RTCRayHit rayHit;

	int depth = 0;

	do 
	{
		rayHit.ray.org_x = ray.org.GetX();
		rayHit.ray.org_y = ray.org.GetY();
		rayHit.ray.org_z = ray.org.GetZ();
		rayHit.ray.dir_x = ray.dir.GetX();
		rayHit.ray.dir_y = ray.dir.GetY();
		rayHit.ray.dir_z = ray.dir.GetZ();
		rayHit.ray.tnear = 0.001f;
		rayHit.ray.tfar = FLT_MAX;
		rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
		rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
		rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

		rtcIntersect1(scene, &context, &rayHit);

		if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
		{
			Ray scattered;
			Vector3 attenuation;

			hit.pos = ray.Eval(rayHit.ray.tfar);
			hit.t = rayHit.ray.tfar;
			hit.normal = Normalize(Vector3(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z));

			if (!materialSet.Scatter(rayHit.hit.geomID, ray, hit, attenuation, scattered, state))
			{
				color *= Vector3(kZero);
				break;
			}

			ray = scattered;
			color *= attenuation;
			++s_totalRays;
		}
	} while (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && (depth++ < 50));

	Vector3 unitDir = Normalize(ray.Direction());
	float t = 0.5f * (unitDir.GetY() + 1.0f);
	color *= (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.5f, 0.7f, 1.0f);

	return color;
}

void RenderSinglePixel(const RTCScene& scene, const Camera& camera, Image& image, uint32_t& state, int i, int j)
{
	Vector3 color(kZero);

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	Ray scattered;
	Hit hit;

	for (int s = 0; s < NUM_SAMPLES; ++s)
	{
		float u = (float(i) + g_RNG.NextFloat()) * image.GetInvWidth();
		float v = (float(j) + g_RNG.NextFloat()) * image.GetInvHeight();

		auto ray = camera.GetRay(u, v, state);

		if constexpr(g_recursive)
		{
			color += GetColor_Recursive(ray, scene, 0, state);
		}
		else
		{
			color += GetColor_Iterative(ray, scene, state);
		}
	}

	color = color * INV_SAMPLES;

	image.SetPixel(i, j, LinearToSRGB(color));
}


void RenderTile(const RTCScene& scene, const Camera& camera, int tileIndex, int numTilesX, int numTilesY, Image& image)
{
	const int tileY = tileIndex / numTilesX;
	const int tileX = tileIndex - tileY * numTilesX;
	const int xStart = tileX * TILE_WIDTH;
	const int xEnd = min(xStart + TILE_WIDTH, IMAGE_WIDTH);
	const int yStart = tileY * TILE_HEIGHT;
	const int yEnd = min(yStart + TILE_HEIGHT, IMAGE_HEIGHT);

	for (int j = yEnd - 1; j >= yStart; --j)
	{
		uint32_t state = (j * 9781 + tileIndex * 6271) | 1;
		for (int i = xStart; i < xEnd; ++i)
		{
			RenderSinglePixel(scene, camera, image, state, i, j);
		}
	}
}


void RenderImageSerial(const RTCScene& scene, const Camera& camera, Image& image)
{
	for (int j = IMAGE_HEIGHT - 1; j >= 0; --j)
	{
		uint32_t state = (j * 9781) | 1;
		for (int i = 0; i < IMAGE_WIDTH; ++i)
		{
			RenderSinglePixel(scene, camera, image, state, i, j);
		}
	}
}


void RenderImageThreaded(const RTCScene& scene, const Camera& camera, Image& image)
{
	constexpr int numTilesX = (IMAGE_WIDTH + TILE_WIDTH - 1) / TILE_WIDTH;
	constexpr int numTilesY = (IMAGE_HEIGHT + TILE_HEIGHT - 1) / TILE_HEIGHT;

	parallel_for(0, numTilesX * numTilesY, [&](int tileIndex)
	{
		RenderTile(scene, camera, tileIndex, numTilesX, numTilesY, image);
	});
}


int main()
{
	// Initialize Embree
	RTCDevice embreeDevice = rtcNewDevice(nullptr);
	RTCScene embreeScene = rtcNewScene(embreeDevice);

	Timer timer;
	timer.Start();

	uint32_t seed = 1524374227u;	// Generated from SetSeedPIDTime
	g_RNG.SetSeed(seed);
	//uint32_t seed = g_RNG.SetSeedPIDTime();

	Image image(IMAGE_WIDTH, IMAGE_HEIGHT);

	// Setup camera
	Camera camera;
	Vector3 cameraPos(13.0f, 2.0f, 3.0f);
	Vector3 cameraTarget(0.0f, 0.0f, 0.0f);
	float distToFocus = 10.0f;
	float aperture = 0.1f;
	camera.LookAt(cameraPos, cameraTarget, Vector3(kYUnitVector), 20.0f, ASPECT, aperture, distToFocus);

	// Generate random scene
	RandomScene();

	// Build Embree scene
	BuildEmbreeScene(embreeDevice, embreeScene);

	// Ray trace
	if (g_threaded)
	{
		RenderImageThreaded(embreeScene, camera, image);
	}
	else
	{
		RenderImageSerial(embreeScene, camera, image);
	}

	timer.Stop();
	double rayCastSeconds = timer.GetElapsedSeconds();

	image.SaveAs("image_embree.ppm");

	// Calculate stats
	size_t primaryRays = IMAGE_WIDTH * IMAGE_HEIGHT * NUM_SAMPLES;
	double primaryRaysPerSecond = (double)primaryRays / rayCastSeconds;
	double totalRaysPerSecond = (double)s_totalRays / rayCastSeconds;

	// Log stats
	stringstream sstr;
	sstr.precision(12);
	sstr << "Ray cast time: " << rayCastSeconds << endl;
	sstr << "  Image size: " << IMAGE_WIDTH << " x " << IMAGE_HEIGHT << " (" << NUM_SAMPLES << " samples per pixel)" << endl;
	sstr << "  Primary rays per second: " << primaryRaysPerSecond << ", primary rays: " << primaryRays << endl;
	sstr << "  Total rays per second: " << totalRaysPerSecond << ", total rays " << s_totalRays << endl;
	OutputDebugStringA(sstr.str().c_str());

	// Clean up Embree
	rtcReleaseScene(embreeScene);
	embreeScene = nullptr;

	rtcReleaseDevice(embreeDevice);
	embreeDevice = nullptr;

	return 0;
}