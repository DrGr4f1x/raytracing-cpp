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


SphereAccelerator::SphereAccelerator()
{
#if USE_SSE4
	// TODO: Check cpuid to make sure we can do SSE 4.2
	m_simdSize = 1;
#endif
}


void SphereAccelerator::AddSphere(const Vector3& center, float radius, uint32_t id)
{
	m_centerX.push_back(center.GetX());
	m_centerY.push_back(center.GetY());
	m_centerZ.push_back(center.GetZ());
	m_radiusSq.push_back(radius * radius);
	m_invRadius.push_back(1.0f / radius);
	m_id.push_back(id);

	m_dirty = true;
}


bool SphereAccelerator::Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	assert(!m_dirty);

	if (m_simdSize == 1)
	{
		return IntersectSpheres1(ray, tMin, tMax, hit);
	}
	else if (m_simdSize == 4)
	{
		return IntersectSpheres4(ray, tMin, tMax, hit);
	}
	else if (m_simdSize == 8)
	{
		assert(false);
		return false;
	}
	else
	{
		assert(false);
		return false;
	}
}


void SphereAccelerator::Commit()
{
	size_t curSize = m_centerX.size();
	size_t targetSize = AlignUp(curSize, m_simdSize);

	// Pad out the sphere arrays with dummy data until we're a multiple of m_simdSize
	for (size_t i = curSize; i < targetSize; ++i)
	{
		m_centerX.push_back(0.0f);
		m_centerY.push_back(0.0f);
		m_centerZ.push_back(0.0f);
		m_radiusSq.push_back(0.0f);
		m_invRadius.push_back(std::numeric_limits<float>::quiet_NaN());
		m_id.push_back(0xFFFFFFFF); // TODO: Make this a constant somewhere
	}

	m_dirty = false;
}


bool SphereAccelerator::IntersectSpheres1(const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	bool anyHit = false;
	Hit tempHit;
	float closestHit = tMax;

	for (size_t i = 0; i < m_centerX.size(); ++i)
	{
		if (IntersectSphere1(i, ray, tMin, closestHit, tempHit))
		{
			anyHit = true;
			closestHit = tempHit.t;
			hit = tempHit;
		}
	}

	if (anyHit)
	{
		hit.pos = ray.Eval(hit.t);
		hit.normal = (hit.pos - Vector3(m_centerX[hit.geomId], m_centerY[hit.geomId], m_centerZ[hit.geomId])) * m_invRadius[hit.geomId];
	}

	return anyHit;
}


bool SphereAccelerator::IntersectSpheres4(const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	Float4 f4RayOrigX = Shuffle<0>(float(ray.org.GetX()));
	Float4 f4RayOrigY = Shuffle<1>(float(ray.org.GetY()));
	Float4 f4RayOrigZ = Shuffle<2>(float(ray.org.GetZ()));
	Float4 f4RayDirX = Shuffle<0>(float(ray.dir.GetX()));
	Float4 f4RayDirY = Shuffle<1>(float(ray.dir.GetY()));
	Float4 f4RayDirZ = Shuffle<2>(float(ray.dir.GetZ()));

	Float4 f4TMin(tMin);
	Float4 f4HitT(tMax);

	UInt4 u4Id(0xffffffff);

	const size_t numSpheres = m_centerX.size();
	for (size_t i = 0; i < numSpheres; i += 4)
	{
		// Load data for 4 spheres
		Float4 f4CenterX = Float4::LoadU(m_centerX.data() + i);
		Float4 f4CenterY = Float4::LoadU(m_centerY.data() + i);
		Float4 f4CenterZ = Float4::LoadU(m_centerZ.data() + i);
		Float4 f4RadiusSq = Float4::LoadU(m_radiusSq.data() + i);

		UInt4 u4CurId = UInt4::LoadU(m_id.data() + i);

		Float4 ocX = f4RayOrigX - f4CenterX;
		Float4 ocY = f4RayOrigY - f4CenterY;
		Float4 ocZ = f4RayOrigZ - f4CenterZ;

		//Float4 a = (f4RayDirX * f4RayDirX) + (f4RayDirY * f4RayDirY) + (f4RayDirZ * f4RayDirZ);
		Float4 b = (ocX * f4RayDirX) + (ocY * f4RayDirY) + (ocZ * f4RayDirZ);
		Float4 c = (ocX * ocX) + (ocY * ocY) + (ocZ * ocZ) - f4RadiusSq;

		Float4 discriminant = (b * b) - c;
		Bool4 discrPos = discriminant > Float4(0.0f);

		if (Any(discrPos))
		{
			Float4 discrSqrt = Sqrt(discriminant);

			Float4 t0 = (-b - discrSqrt);
			Float4 t1 = (-b + discrSqrt);

			Float4 t = Select(t0 > f4TMin, t0, t1);
			Bool4 mask = discrPos & (t > f4TMin) & (t < f4HitT);

			u4Id = Select(mask, u4CurId, u4Id);
			f4HitT = Select(mask, t, f4HitT);
		}
	}

	float minT = ReduceMin(f4HitT);
	if (minT < tMax)
	{
		uint32_t minMask = Mask(f4HitT == Float4(minT));
		if (minMask != 0x0)
		{
			uint32_t ids[4];
			float hits[4];
			UInt4::StoreU(ids, u4Id);
			Float4::StoreU(hits, f4HitT);

			hit.t = hits[0];
			hit.geomId = ids[0];
			for (int i = 1; i < 4; ++i)
			{
				if (hits[i] < hit.t)
				{
					hit.t = hits[i];
					hit.geomId = ids[i];
				}
			}

			hit.pos = ray.Eval(hit.t);
			hit.normal = (hit.pos - Vector3(m_centerX[hit.geomId], m_centerY[hit.geomId], m_centerZ[hit.geomId])) * m_invRadius[hit.geomId];

			return true;
		}
	}

	return false;
}


bool SphereAccelerator::IntersectSphere1(size_t index, const Ray& ray, float tMin, float tMax, Hit& hit) const
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
			hit.geomId = m_id[index];
			return true;
		}
		temp = (-b + discrSqrt) / a;
		if (temp < tMax && temp > tMin)
		{
			hit.t = temp;
			hit.geomId = m_id[index];
			return true;
		}
	}

	return false;
}