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
#include "Timer.h"
#include "Math\Random.h"

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
constexpr bool g_recursive = false;

// Stats
atomic_size_t s_totalRays = 0;

MaterialSet materialSet;

Vector3 GetColor_Recursive(Ray& ray, const Scene& scene, int depth, uint32_t& state)
{
	Hit hit;
	++s_totalRays;

	scene.Intersect1(ray, hit);

	if(hit.geomId != 0xFFFFFFFF)
	{
		Ray scattered;
		Vector3 attenuation;
		if (depth < MAX_RECURSION && materialSet.Scatter(ray, hit, attenuation, scattered, state))
		{
			return attenuation * GetColor_Recursive(scattered, scene, depth + 1, state);
		}
		else
		{
			return Vector3(kZero);
		}
	}
	
	Vector3 unitDir = Normalize(Vector3(ray.dirX, ray.dirY, ray.dirZ));
	float t = 0.5f * (unitDir.GetY() + 1.0f);
	return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.5f, 0.7f, 1.0f);
}

Vector3 GetColor_Iterative(Ray& ray, const Scene& scene, uint32_t& state)
{
	Vector3 color(kOne);

	Hit hit;
	int depth = 0;
	++s_totalRays;
	do
	{
		hit.geomId = 0xFFFFFFFF;
		scene.Intersect1(ray, hit);

		if (hit.geomId != 0xFFFFFFFF)
		{
			++depth;
			Ray scattered;
			Vector3 attenuation;

			if (!materialSet.Scatter(ray, hit, attenuation, scattered, state))
			{
				color *= Vector3(kZero);
				break;
			}

			color *= attenuation;
			ray = scattered;
			++s_totalRays;
		}
	} while (hit.geomId != 0xFFFFFFFF && depth < 50);

	Vector3 unitDir = Normalize(Vector3(ray.dirX, ray.dirY, ray.dirZ));
	float t = 0.5f * (unitDir.GetY() + 1.0f);
	color *= (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.5f, 0.7f, 1.0f);

	return color;
}

Vector3 LinearToSRGB(Vector3 linearRGB)
{
	XMVECTOR T = XMVectorSaturate(linearRGB);
	XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(1.0f / 2.4f)), 1.055f), XMVectorReplicate(0.055f));
	result = XMVectorSelect(result, XMVectorScale(T, 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
	return Vector3(XMVectorSelect(T, result, g_XMSelect1110));
}


void RandomScene(Scene& scene, RandomNumberGenerator& rng)
{
	uint32_t id = 0;
	scene.AddSphere(Vector3(0.0f, -1000.0f, 0.0f), 1000.0f, id++);
	materialSet.AddLambertian(Vector3(0.5f, 0.5f, 0.5f));

	for (int a = -SPHERE_GRID_SIZE; a < SPHERE_GRID_SIZE; ++a)
	{
		for (int b = -SPHERE_GRID_SIZE; b < SPHERE_GRID_SIZE; ++b)
		{
			float radius = 0.2f + rng.NextFloat(-0.1f, 0.1f);
			Vector3 center(a + 0.9f * rng.NextFloat(), radius, b + 0.9f * rng.NextFloat());

			if (Length(center - Vector3(4.0f, 0.2f, 0.0f)) > 0.9f)
			{				
				float chooseMat = rng.NextFloat();

				if (chooseMat < 0.8f) // Lambertian
				{
					materialSet.AddLambertian(Vector3(rng.NextFloat() * rng.NextFloat(), rng.NextFloat() * rng.NextFloat(), rng.NextFloat() * rng.NextFloat()));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					materialSet.AddMetallic(Vector3(0.5f * (1.0f + rng.NextFloat()), 0.5f * (1.0f + rng.NextFloat()), 0.5f * (1.0f + rng.NextFloat())), 0.5f * rng.NextFloat());
				}
				else // Dielectric
				{
					materialSet.AddDielectric(1.5f);
				}

				scene.AddSphere(center, radius, id++);
			}
		}
	}

	scene.AddSphere(Vector3(0.0f, 1.0f, 0.0f), 1.0f, id++);
	materialSet.AddDielectric(1.5f);

	scene.AddSphere(Vector3(-4.0f, 1.0f, 0.0f), 1.0f, id++);
	materialSet.AddLambertian(Vector3(0.4f, 0.2f, 0.1f));

	scene.AddSphere(Vector3(4.0f, 1.0f, 0.0f), 1.0f, id++);
	materialSet.AddMetallic(Vector3(0.7f, 0.6f, 0.5f), 0.0f);

	scene.Commit();
}


void RenderSinglePixel(const Scene& scene, const Camera& camera, Image& image, uint32_t& state, int i, int j)
{
	Vector3 color(kZero);
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


void RenderTile(const Scene& scene, const Camera& camera, int tileIndex, int numTilesX, int numTilesY, Image& image)
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


void RenderImageSerial(const Scene& scene, const Camera& camera, Image& image)
{
	constexpr int numTilesX = (IMAGE_WIDTH + TILE_WIDTH - 1) / TILE_WIDTH;
	constexpr int numTilesY = (IMAGE_HEIGHT + TILE_HEIGHT - 1) / TILE_HEIGHT;

	for (int iTile = 0; iTile < numTilesX * numTilesY; ++iTile)
	{
		RenderTile(scene, camera, iTile, numTilesX, numTilesY, image);
	}
}


void RenderImageThreaded(const Scene& scene, const Camera& camera, Image& image)
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
	Scene scene;
	RandomScene(scene, g_RNG);

	if constexpr(g_threaded)
	{
		RenderImageThreaded(scene, camera, image);
	}
	else
	{
		RenderImageSerial(scene, camera, image);
	}

	timer.Stop();
	double rayCastSeconds = timer.GetElapsedSeconds();

	image.SaveAs("image.ppm");

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

	return 0;
}