//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "Camera.h"
#include "Dielectric.h"
#include "Image.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Sampling.h"
#include "Scene.h"
#include "Sphere.h"
#include "Timer.h"
#include "Math\Random.h"

using namespace std;
using namespace concurrency;
using namespace Math;


constexpr int IMAGE_WIDTH = 1280;
constexpr int IMAGE_HEIGHT = 720;
constexpr int NUM_SAMPLES = 256;
constexpr float INV_SAMPLES = 1.0f / static_cast<float>(NUM_SAMPLES);
constexpr float ASPECT = static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);
constexpr int MAX_RECURSION = 50;


Vector3 GetColor(const Ray& ray, const Scene& scene, int depth)
{
	Hit hit;
	if (scene.Intersect(ray, 0.001f, FLT_MAX, hit))
	{
		Ray scattered;
		Vector3 attenuation;
		if (depth < MAX_RECURSION && hit.material->Scatter(ray, hit, attenuation, scattered, g_RNG))
		{
			return attenuation * GetColor(scattered, scene, depth + 1);
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


Vector3 LinearToSRGB(Vector3 linearRGB)
{
	XMVECTOR T = XMVectorSaturate(linearRGB);
	XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(1.0f / 2.4f)), 1.055f), XMVectorReplicate(0.055f));
	result = XMVectorSelect(result, XMVectorScale(T, 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
	return Vector3(XMVectorSelect(T, result, g_XMSelect1110));
}


void RandomScene(Scene& scene, RandomNumberGenerator& rng)
{
	auto sphere = Sphere::Make(Vector3(0.0f, -1000.0f, 0.0f), 1000.0f);
	sphere->SetMaterial(Lambertian::Make(Vector3(0.5f, 0.5f, 0.5f)));
	scene.AddPrimitive(sphere);

	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			float radius = 0.2f + rng.NextFloat(-0.1f, 0.1f);
			Vector3 center(a + 0.9f * rng.NextFloat(), radius, b + 0.9f * rng.NextFloat());

			if (Length(center - Vector3(4.0f, 0.2f, 0.0f)) > 0.9f)
			{
				sphere = Sphere::Make(center, radius);
				
				float chooseMat = rng.NextFloat();

				if (chooseMat < 0.8f) // Lambertian
				{
					auto material = Lambertian::Make(Vector3(rng.NextFloat() * rng.NextFloat(), rng.NextFloat() * rng.NextFloat(), rng.NextFloat() * rng.NextFloat()));
					sphere->SetMaterial(material);
				}
				else if (chooseMat < 0.95f) // Metal
				{
					auto material = Metal::Make(Vector3(0.5f * (1.0f + rng.NextFloat()), 0.5f * (1.0f + rng.NextFloat()), 0.5f * (1.0f + rng.NextFloat())), 0.5f * rng.NextFloat());
					sphere->SetMaterial(material);
				}
				else // Dielectric
				{
					auto material = Dielectric::Make(1.5f);
					sphere->SetMaterial(material);
				}

				scene.AddPrimitive(sphere);
			}
		}
	}

	sphere = Sphere::Make(Vector3(0.0f, 1.0f, 0.0f), 1.0f);
	sphere->SetMaterial(Dielectric::Make(1.5f));
	scene.AddPrimitive(sphere);

	sphere = Sphere::Make(Vector3(-4.0f, 1.0f, 0.0f), 1.0f);
	sphere->SetMaterial(Lambertian::Make(Vector3(0.4f, 0.2f, 0.1f)));
	scene.AddPrimitive(sphere);

	sphere = Sphere::Make(Vector3(4.0f, 1.0f, 0.0f), 1.0f);
	sphere->SetMaterial(Metal::Make(Vector3(0.7f, 0.6f, 0.5f), 0.0f));
	scene.AddPrimitive(sphere);
}

void RenderTile(const Scene& scene, int tileIndex)
{

}

void RenderImage(const Scene& scene, const uint32_t imageWidth, const uint32_t imageHeight)
{

}

int main()
{
	Timer timer;
	timer.Start();

	g_RNG.SetSeedPIDTime();

	Image image(IMAGE_WIDTH, IMAGE_HEIGHT);

	Camera camera;
	Vector3 cameraPos(13.0f, 2.0f, 3.0f);
	Vector3 cameraTarget(0.0f, 0.0f, 0.0f);
	float distToFocus = 10.0f;
	float aperture = 0.1f;
	camera.LookAt(cameraPos, cameraTarget, Vector3(kYUnitVector), 20.0f, ASPECT, aperture, distToFocus);

	Scene scene;
	RandomScene(scene, g_RNG);

	for (int j = IMAGE_HEIGHT - 1; j >= 0; --j)
	{
		for (int i = 0; i < IMAGE_WIDTH; ++i)
		{
			Vector3 color(kZero);
			for (int s = 0; s < NUM_SAMPLES; ++s)
			{
				float u = (float(i) + g_RNG.NextFloat()) * image.GetInvWidth();
				float v = (float(j) + g_RNG.NextFloat()) * image.GetInvHeight();

				auto ray = camera.GetRay(u, v, g_RNG);
				color += GetColor(ray, scene, 0);
			}
			
			color = color * INV_SAMPLES;

			image.SetPixel(i, j, LinearToSRGB(color));
		}
	}

	timer.Sample();
	double rayCastSeconds = timer.GetElapsedSeconds();

	image.SaveAs("image.ppm");

	timer.Stop();
	double fileIOSeconds = timer.GetElapsedSeconds();

	double totalRays = IMAGE_WIDTH * IMAGE_HEIGHT * NUM_SAMPLES;
	double raysPerSecond = totalRays / rayCastSeconds;

	stringstream sstr;
	sstr << "Ray cast time: " << rayCastSeconds << ", rays per second: " << raysPerSecond << ", total rays " << totalRays << endl;
	OutputDebugStringA(sstr.str().c_str());

	return 0;
}