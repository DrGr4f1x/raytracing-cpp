//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "ConeAccel.h"


using namespace Math;
using namespace std;


template<int N>
bool IntersectCones(const ConeList& coneList, Ray& ray, Hit& hit)
{
	return false;
}


bool IntersectCone1(const ConeList& coneList, size_t index, Ray& ray, Hit& hit)
{
	return false;
}

template <>
bool IntersectCones<1>(const ConeList& coneList, Ray& ray, Hit& hit)
{
	bool anyHit = false;
	Hit tempHit;
	float closestHit = ray.tmax;

	for (size_t i = 0; i < coneList.centerX.size(); ++i)
	{
		if (IntersectCone1(coneList, i, ray, tempHit))
		{
			anyHit = true;
			closestHit = ray.tmax;
			hit = tempHit;
		}
	}

	if (anyHit)
	{
		hit.pos = ray.Eval(ray.tmax);
		hit.normal = (hit.pos - coneList.Center(hit.geomId));
	}

	return anyHit;
}


ConeAccelerator::ConeAccelerator()
{
	// TODO: Check cpuid to make sure we support the selected SIMD ISA
#if USE_AVX || USE_AVX2
	m_simdSize = 8;
#elif USE_SSE4
	m_simdSize = 4;
#endif
}


void ConeAccelerator::AddCone(const Vector3& center, float radius, float height, uint32_t id)
{
	m_coneList.centerX.push_back(center.GetX());
	m_coneList.centerY.push_back(center.GetY());
	m_coneList.centerZ.push_back(center.GetZ());
	m_coneList.radius.push_back(radius);
	m_coneList.height.push_back(height);
	m_coneList.id.push_back(id);

	m_dirty = true;
}


bool ConeAccelerator::Intersect(Ray& ray, Hit& hit) const
{
	assert(!m_dirty);

	if (m_simdSize == 1)
	{
		return IntersectCones<1>(m_coneList, ray, hit);
	}
	else if (m_simdSize == 4)
	{
		return IntersectCones<1>(m_coneList, ray, hit);
	}
	else if (m_simdSize == 8)
	{
		return IntersectCones<1>(m_coneList, ray, hit);
	}
	else
	{
		assert(false);
		return false;
	}
}


void ConeAccelerator::Commit()
{
	size_t curSize = m_coneList.centerX.size();
	size_t targetSize = AlignUp(curSize, m_simdSize);

	// Pad out the sphere arrays with dummy data until we're a multiple of m_simdSize
	for (size_t i = curSize; i < targetSize; ++i)
	{
		m_coneList.centerX.push_back(0.0f);
		m_coneList.centerY.push_back(0.0f);
		m_coneList.centerZ.push_back(0.0f);
		m_coneList.radius.push_back(0.0f);
		m_coneList.height.push_back(0.0f);
		m_coneList.id.push_back(0xFFFFFFFF); // TODO: Make this a constant somewhere
	}

	m_dirty = false;
}