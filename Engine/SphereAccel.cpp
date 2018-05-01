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
	// TODO: Check cpuid to make sure we support the selected SIMD ISA
#if USE_AVX || USE_AVX2
	m_simdSize = 8;
#elif USE_SSE4
	m_simdSize = 4;
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
		return IntersectSpheres8(ray, tMin, tMax, hit);
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
	Float4 f4RayOrigX = Float4::Broadcast(ray.org.GetX());
	Float4 f4RayOrigY = Float4::Broadcast(ray.org.GetY());
	Float4 f4RayOrigZ = Float4::Broadcast(ray.org.GetZ());
	Float4 f4RayDirX = Float4::Broadcast(ray.dir.GetX());
	Float4 f4RayDirY = Float4::Broadcast(ray.dir.GetY());
	Float4 f4RayDirZ = Float4::Broadcast(ray.dir.GetZ());

	Float4 f4TMin(tMin);
	Float4 f4HitT(tMax);

	UInt4 u4Id(0xffffffff);

	const size_t numSpheres = m_centerX.size();
	for (size_t i = 0; i < numSpheres; i += m_simdSize)
	{
		// Load data for 4 spheres
		Float4 f4CenterX = Float4::Load(m_centerX.data() + i);
		Float4 f4CenterY = Float4::Load(m_centerY.data() + i);
		Float4 f4CenterZ = Float4::Load(m_centerZ.data() + i);
		Float4 f4RadiusSq = Float4::Load(m_radiusSq.data() + i);

		UInt4 u4CurId = UInt4::Load(m_id.data() + i);

		Float4 ocX = f4RayOrigX - f4CenterX;
		Float4 ocY = f4RayOrigY - f4CenterY;
		Float4 ocZ = f4RayOrigZ - f4CenterZ;

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
			for (int i = 1; i < m_simdSize; ++i)
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


bool SphereAccelerator::IntersectSpheres8(const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	Float8 f8RayOrigX = Float8::Broadcast(ray.org.GetX());
	Float8 f8RayOrigY = Float8::Broadcast(ray.org.GetY());
	Float8 f8RayOrigZ = Float8::Broadcast(ray.org.GetZ());
	Float8 f8RayDirX = Float8::Broadcast(ray.dir.GetX());
	Float8 f8RayDirY = Float8::Broadcast(ray.dir.GetY());
	Float8 f8RayDirZ = Float8::Broadcast(ray.dir.GetZ());

	Float8 f8TMin(tMin);
	Float8 f8HitT(tMax);

	UInt8 u8Id(0xffffffff);

	const size_t numSpheres = m_centerX.size();
	for (size_t i = 0; i < numSpheres; i += m_simdSize)
	{
		// Load data for 8 spheres
		Float8 f8CenterX = Float8::Load(m_centerX.data() + i);
		Float8 f8CenterY = Float8::Load(m_centerY.data() + i);
		Float8 f8CenterZ = Float8::Load(m_centerZ.data() + i);
		Float8 f8RadiusSq = Float8::Load(m_radiusSq.data() + i);

		UInt8 u8CurId = UInt8::Load(m_id.data() + i);

		Float8 ocX = f8RayOrigX - f8CenterX;
		Float8 ocY = f8RayOrigY - f8CenterY;
		Float8 ocZ = f8RayOrigZ - f8CenterZ;

		Float8 b = (ocX * f8RayDirX) + (ocY * f8RayDirY) + (ocZ * f8RayDirZ);
		Float8 c = (ocX * ocX) + (ocY * ocY) + (ocZ * ocZ) - f8RadiusSq;

		Float8 discriminant = (b * b) - c;
		Bool8 discrPos = discriminant > Float8(0.0f);

		if (Any(discrPos))
		{
			Float8 discrSqrt = Sqrt(discriminant);

			Float8 t0 = (-b - discrSqrt);
			Float8 t1 = (-b + discrSqrt);

			Float8 t = Select(t0 > f8TMin, t0, t1);
			Bool8 mask = discrPos & (t > f8TMin) & (t < f8HitT);

			u8Id = Select(mask, u8CurId, u8Id);
			f8HitT = Select(mask, t, f8HitT);
		}
	}

	float minT = ReduceMin(f8HitT);
	if (minT < tMax)
	{
		uint32_t minMask = Mask(f8HitT == Float8(minT));
		if (minMask != 0x0)
		{
			uint32_t ids[8];
			float hits[8];
			UInt8::StoreU(ids, u8Id);
			Float8::StoreU(hits, f8HitT);

			hit.t = hits[0];
			hit.geomId = ids[0];
			for (int i = 1; i < m_simdSize; ++i)
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