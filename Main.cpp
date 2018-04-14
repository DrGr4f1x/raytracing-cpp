//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "Camera.h"
#include "Scene.h"
#include "Sphere.h"
#include "Timer.h"


using namespace std;
using namespace Math;


bool HitSphere(const Vector3 center, float radius, const Ray& ray)
{
	Vector3 oc = ray.Origin() - center;
	float a = Dot(ray.Direction(), ray.Direction());
	float b = 2.0f * Dot(oc, ray.Direction());
	float c = Dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4.0f * a * c;
	return (discriminant > 0.0f);
}

Vector3 GetColor(const Ray& ray, const Scene& scene)
{
	Hit hit;
	if (scene.Intersect(ray, 0.0f, FLT_MAX, hit))
	{
		return 0.5f * (hit.normal + Vector3(1.0f, 1.0f, 1.0f));
	}
	
	Vector3 unitDir = Normalize(ray.Direction());
	float t = 0.5f * (unitDir.GetY() + 1.0f);
	return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.5f, 0.7f, 1.0f);
}

int main()
{
	Timer timer;
	timer.Start();

	constexpr int nx = 1280;
	constexpr int ny = 720;
	constexpr float invX = 1.0f / float(nx);
	constexpr float invY = 1.0f / float(ny);

	auto image = make_unique<Vector3[]>(nx * ny);

	auto coordToIndex = [nx](int i, int j) -> int
	{
		return i + j * nx;
	};

	Camera camera;
	camera.LookAt(Vector3(kZero), Vector3(0.0f, 0.0f, -1.0f), Vector3(kYUnitVector), 90.0f, float(nx) / float(ny));

	Scene scene;
	Sphere sphere0{ Vector3(0.0f, 0.0f, -1.0f), 0.5f };
	Sphere sphere1{ Vector3(0.0f, -100.5f, -1.0f), 100.0f };

	scene.AddPrimitive(&sphere0);
	scene.AddPrimitive(&sphere1);

	for (int j = ny - 1; j >= 0; --j)
	{
		for (int i = 0; i < nx; ++i)
		{
			float u = float(i) * invX;
			float v = float(j) * invY;

			auto ray = camera.GetRay(u, v);
			image[coordToIndex(i, j)] = GetColor(ray, scene);
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