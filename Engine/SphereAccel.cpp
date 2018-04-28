//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "SphereAccel.h"


using namespace Math;
using namespace std;


void SphereAccelerator::AddSphere(const Vector3& center, float radius, uint32_t id)
{
	m_centerX.push_back(center.GetX());
	m_centerY.push_back(center.GetY());
	m_centerZ.push_back(center.GetZ());
	m_radiusSq.push_back(radius * radius);
	m_invRadius.push_back(1.0f / radius);
	m_id.push_back(id);
}


bool SphereAccelerator::Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	bool anyHit = false;
	Hit tempHit;
	float closestHit = tMax;

	for (size_t i = 0; i < m_centerX.size(); ++i)
	{
		if (IntersectSingleSphere(i, ray, tMin, closestHit, tempHit))
		{
			anyHit = true;
			closestHit = tempHit.t;
			hit = tempHit;
		}
	}

	return anyHit;
}


bool SphereAccelerator::IntersectSingleSphere(size_t index, const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	Vector3 center = Vector3(m_centerX[index], m_centerY[index], m_centerZ[index]);
	float radiusSq = m_radiusSq[index];

	Vector3 oc = ray.Origin() - center;
	float a = Dot(ray.Direction(), ray.Direction());
	float b = Dot(oc, ray.Direction());
	float c = Dot(oc, oc) - radiusSq;
	float discriminant = b * b - a * c;

	if (discriminant > 0.0f)
	{
		float invRadius = m_invRadius[index];
		float discrSqrt = sqrtf(discriminant);

		float temp = (-b - discrSqrt) / a;
		if (temp < tMax && temp > tMin)
		{
			hit.t = temp;
			hit.pos = ray.Eval(hit.t);
			hit.normal = (hit.pos - center) * invRadius;
			hit.geomId = m_id[index];
			return true;
		}
		temp = (-b + discrSqrt) / a;
		if (temp < tMax && temp > tMin)
		{
			hit.t = temp;
			hit.pos = ray.Eval(hit.t);
			hit.normal = (hit.pos - center) * invRadius;
			hit.geomId = m_id[index];
			return true;
		}
	}

	return false;
}