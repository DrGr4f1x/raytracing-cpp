//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "Sphere.h"


using namespace std;
using namespace Math;

static uint32_t s_geomId{ 0 };

shared_ptr<IPrimitive> Sphere::Make(Vector3 center, float radius)
{
	return make_shared<Sphere>(center, radius, s_geomId++);
}


bool Sphere::Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	Vector3 oc = ray.Origin() - m_center;
	float a = Dot(ray.Direction(), ray.Direction());
	float b = Dot(oc, ray.Direction());
	float c = Dot(oc, oc) - m_radius * m_radius;
	float discriminant = b * b - a * c;
	
	if (discriminant > 0.0f)
	{
		float temp = (-b - sqrtf(discriminant)) / a;
		if (temp < tMax && temp > tMin)
		{
			hit.t = temp;
			hit.pos = ray.Eval(hit.t);
			hit.normal = (hit.pos - m_center) / m_radius;
			hit.geomId = m_geomId;
			return true;
		}
		temp = (-b + sqrtf(discriminant)) / a;
		if (temp < tMax && temp > tMin)
		{
			hit.t = temp;
			hit.pos = ray.Eval(hit.t);
			hit.normal = (hit.pos - m_center) / m_radius;
			hit.geomId = m_geomId;
			return true;
		}
	}

	return false;
}