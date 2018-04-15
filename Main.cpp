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
#include "Lambertian.h"
#include "Metal.h"
#include "Sampling.h"
#include "Scene.h"
#include "Sphere.h"
#include "Timer.h"
#include "Math\Random.h"

using namespace std;
using namespace Math;

RandomNumberGenerator g_rng;

Vector3 GetColor(const Ray& ray, const Scene& scene, int depth)
{
	Hit hit;
	if (scene.Intersect(ray, 0.00001f, FLT_MAX, hit))
	{
		Ray scattered;
		Vector3 attenuation;
		if (depth < 50 && hit.material->Scatter(ray, hit, attenuation, scattered, g_rng))
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
	/*XMVECTOR T = XMVectorSaturate(linearRGB);
	XMVECTOR result = XMVectorSubtract(XMVectorScale(XMVectorPow(T, XMVectorReplicate(1.0f / 2.4f)), 1.055f), XMVectorReplicate(0.055f));
	result = XMVectorSelect(result, XMVectorScale(T, 12.92f), XMVectorLess(T, XMVectorReplicate(0.0031308f)));
	return Vector3(XMVectorSelect(T, result, g_XMSelect1110));*/

	return Vector3(sqrtf(linearRGB.GetX()), sqrtf(linearRGB.GetY()), sqrtf(linearRGB.GetZ()));
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

int main()
{
	Timer timer;
	timer.Start();

	g_rng.SetSeedPIDTime();

	constexpr int nx = 1280;
	constexpr int ny = 720;
	constexpr int nsamples = 256;
	constexpr float invX = 1.0f / float(nx);
	constexpr float invY = 1.0f / float(ny);
	constexpr float invSamples = 1.0f / float(nsamples);

	auto image = make_unique<Vector3[]>(nx * ny);

	auto coordToIndex = [nx](int i, int j) -> int
	{
		return i + j * nx;
	};

	Camera camera;
	Vector3 cameraPos(13.0f, 2.0f, 3.0f);
	Vector3 cameraTarget(0.0f, 0.0f, 0.0f);
	float distToFocus = 10.0f;
	float aperture = 0.1f;
	camera.LookAt(cameraPos, cameraTarget, Vector3(kYUnitVector), 20.0f, float(nx) / float(ny), aperture, distToFocus);

	Scene scene;
	RandomScene(scene, g_rng);


	for (int j = ny - 1; j >= 0; --j)
	{
		for (int i = 0; i < nx; ++i)
		{
			Vector3 color(kZero);
			for (int s = 0; s < nsamples; ++s)
			{
				float u = (float(i) + g_rng.NextFloat()) * invX;
				float v = (float(j) + g_rng.NextFloat()) * invY;

				auto ray = camera.GetRay(u, v, g_rng);
				color += GetColor(ray, scene, 0);
			}
			
			color = color * invSamples;

			image[coordToIndex(i, j)] = LinearToSRGB(color);
		}
	}

	timer.Sample();
	double rayCastSeconds = timer.GetElapsedSeconds();

	ofstream outfile;
	outfile.open("image.ppm", ios::out | ios::trunc);

	outfile << "P3\n" << nx << " " << ny << "\n255\n";

	for (int j = ny - 1; j >= 0; --j)
	{
		for (int i = 0; i < nx; ++i)
		{
			const Vector3& color = image[coordToIndex(i, j)];
			int ir = int(255.99f * color.GetX());
			int ig = int(255.99f * color.GetY());
			int ib = int(255.99f * color.GetZ());

			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}

	outfile.close();

	timer.Stop();
	double fileIOSeconds = timer.GetElapsedSeconds();

	stringstream sstr;
	sstr << "Ray cast time: " << rayCastSeconds << ", file I/O time: " << fileIOSeconds << endl;
	OutputDebugStringA(sstr.str().c_str());

	return 0;
}