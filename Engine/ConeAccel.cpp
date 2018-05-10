//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "ConeAccel.h"

#include "Scene.h"


using namespace Math;
using namespace std;


template<int N>
void IntersectCones(const ConeList& coneList, Ray& ray, Hit& hit)
{
}


void IntersectCone1(const ConeList& coneList, size_t index, Ray& ray, Hit& hit)
{
}

template <>
void IntersectCones<1>(const ConeList& coneList, Ray& ray, Hit& hit)
{
	Hit tempHit;
	float closestHit = ray.tmax;

	for (size_t i = 0; i < coneList.GetNumCones(); ++i)
	{
		IntersectCone1(coneList, i, ray, hit);
		if(tempHit.geomId != 0xFFFFFFFF)
		{
			closestHit = ray.tmax;
			hit = tempHit;
		}
	}

	if (tempHit.geomId != 0xFFFFFFFF)
	{
		hit.normalX = (ray.posX + ray.tmax * ray.dirX) - coneList.centerX[hit.geomId];
		hit.normalY = (ray.posY + ray.tmax * ray.dirY) - coneList.centerY[hit.geomId];
		hit.normalZ = (ray.posZ + ray.tmax * ray.dirZ) - coneList.centerZ[hit.geomId];
	}
}


ConeAccelerator::ConeAccelerator(Scene* scene)
	: m_scene(scene)
{}


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


void ConeAccelerator::Intersect1(Ray& ray, Hit& hit) const
{
	assert(!m_dirty);

	const auto simdSize = m_scene->GetSimdSize();

	if (simdSize == 1)
	{
		IntersectCones<1>(m_coneList, ray, hit);
	}
	else if (simdSize == 4)
	{
		IntersectCones<1>(m_coneList, ray, hit);
	}
	else if (simdSize == 8)
	{
		IntersectCones<1>(m_coneList, ray, hit);
	}
}


void ConeAccelerator::Commit()
{
	const auto simdSize = m_scene->GetSimdSize();

	size_t curSize = m_coneList.centerX.size();
	size_t targetSize = AlignUp(curSize, simdSize);

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