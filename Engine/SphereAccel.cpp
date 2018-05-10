//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "SphereAccel.h"

#include "Scene.h"


using namespace Math;
using namespace std;


template<int N>
void IntersectSpheres(const SphereList& sphereList, Ray& ray, Hit& hit)
{
	Float<N> rayOrigX = Float<N>::Broadcast(ray.org.GetX());
	Float<N> rayOrigY = Float<N>::Broadcast(ray.org.GetY());
	Float<N> rayOrigZ = Float<N>::Broadcast(ray.org.GetZ());
	Float<N> rayDirX = Float<N>::Broadcast(ray.dir.GetX());
	Float<N> rayDirY = Float<N>::Broadcast(ray.dir.GetY());
	Float<N> rayDirZ = Float<N>::Broadcast(ray.dir.GetZ());

	Float<N> tmin = Float<N>::Broadcast(ray.tmin);
	Float<N> hitT = Float<N>::Broadcast(ray.tmax);

	UInt<N> id(0xffffffff);

	const size_t numSpheres = sphereList.GetNumSpheres();
	for (size_t i = 0; i < numSpheres; i += N)
	{
		// Load data for 4 spheres
		Float<N> centerX = Float<N>::Load(sphereList.centerX.data() + i);
		Float<N> centerY = Float<N>::Load(sphereList.centerY.data() + i);
		Float<N> centerZ = Float<N>::Load(sphereList.centerZ.data() + i);
		Float<N> radiusSq = Float<N>::Load(sphereList.radiusSq.data() + i);

		UInt<N> curId = UInt<N>::Load(sphereList.id.data() + i);

		Float<N> ocX = rayOrigX - centerX;
		Float<N> ocY = rayOrigY - centerY;
		Float<N> ocZ = rayOrigZ - centerZ;

		Float<N> b = (ocX * rayDirX) + (ocY * rayDirY) + (ocZ * rayDirZ);
		Float<N> c = (ocX * ocX) + (ocY * ocY) + (ocZ * ocZ) - radiusSq;

		Float<N> discriminant = (b * b) - c;
		Bool<N> discrPos = discriminant > Float<N>(0.0f);

		if (Any(discrPos))
		{
			Float<N> discrSqrt = Sqrt(discriminant);

			Float<N> t0 = (-b - discrSqrt);
			Float<N> t1 = (-b + discrSqrt);

			Float<N> t = Select(t0 > tmin, t0, t1);
			Bool<N> mask = discrPos & (t > tmin) & (t < hitT);

			id = Select(mask, curId, id);
			hitT = Select(mask, t, hitT);
		}
	}

	float minT = ReduceMin(hitT);
	if (minT < ray.tmax)
	{
		uint32_t minMask = Mask(hitT == Float<N>(minT));
		if (minMask != 0x0)
		{
			uint32_t ids[N];
			float hits[N];
			UInt<N>::StoreU(ids, id);
			Float<N>::StoreU(hits, hitT);
			
			float bestT = hits[0];
			hit.geomId = ids[0];
			for (int i = 1; i < N; ++i)
			{
				if (hits[i] < bestT)
				{
					bestT = hits[i];
					hit.geomId = ids[i];
				}
			}

			ray.tmax = bestT;
			hit.pos = ray.Eval(bestT);
			hit.normal = (hit.pos - Vector3(sphereList.centerX[hit.geomId], sphereList.centerY[hit.geomId], sphereList.centerZ[hit.geomId])) * sphereList.invRadius[hit.geomId];
		}
	}
}


void IntersectSphere1(const SphereList& sphereList, size_t index, Ray& ray, Hit& hit)
{
	Vector3 center = sphereList.Center(index);
	float radiusSq = sphereList.radiusSq[index];

	Vector3 oc = ray.org - center;
	float b = Dot(oc, ray.dir);
	float c = Dot(oc, oc) - radiusSq;
	float discriminant = b * b - c;

	if (discriminant > 0.0f)
	{
		float invRadius = sphereList.invRadius[index];
		float discrSqrt = sqrtf(discriminant);

		float temp = (-b - discrSqrt);
		if (temp < ray.tmax && temp > ray.tmin)
		{
			ray.tmax = temp;
			hit.geomId = sphereList.id[index];
			return;
		}
		temp = (-b + discrSqrt);
		if (temp < ray.tmax && temp > ray.tmin)
		{
			ray.tmax = temp;
			hit.geomId = sphereList.id[index];
		}
	}
}


template<>
void IntersectSpheres<1>(const SphereList& sphereList, Ray& ray, Hit& hit)
{
	Hit tempHit;
	float closestHit = ray.tmax;

	for (size_t i = 0; i < sphereList.GetNumSpheres(); ++i)
	{
		IntersectSphere1(sphereList, i, ray, tempHit);
		if(tempHit.geomId != 0xFFFFFFFF)
		{
			closestHit = ray.tmax;
			hit = tempHit;
		}
	}

	if (tempHit.geomId != 0xFFFFFFFF)
	{
		hit.pos = ray.Eval(ray.tmax);
		hit.normal = (hit.pos - sphereList.Center(hit.geomId)) * sphereList.invRadius[hit.geomId];
	}
}


SphereAccelerator::SphereAccelerator(Scene* scene)
	: m_scene(scene)
{}


void SphereAccelerator::AddSphere(const Vector3& center, float radius, uint32_t id)
{
	m_sphereList.centerX.push_back(center.GetX());
	m_sphereList.centerY.push_back(center.GetY());
	m_sphereList.centerZ.push_back(center.GetZ());
	m_sphereList.radiusSq.push_back(radius * radius);
	m_sphereList.invRadius.push_back(1.0f / radius);
	m_sphereList.id.push_back(id);

	m_dirty = true;
}


void SphereAccelerator::Intersect1(Ray& ray, Hit& hit) const
{
	assert(!m_dirty);

	const auto simdSize = m_scene->GetSimdSize();

	if (simdSize == 1)
	{
		IntersectSpheres<1>(m_sphereList, ray, hit);
	}
	else if (simdSize == 4)
	{
		IntersectSpheres<4>(m_sphereList, ray, hit);
	}
	else if (simdSize == 8)
	{
		IntersectSpheres<8>(m_sphereList, ray, hit);
	}
}


void SphereAccelerator::Commit()
{
	const auto simdSize = m_scene->GetSimdSize();

	size_t curSize = m_sphereList.GetNumSpheres();
	size_t targetSize = AlignUp(curSize, simdSize);

	// Pad out the sphere arrays with dummy data until we're a multiple of m_simdSize
	for (size_t i = curSize; i < targetSize; ++i)
	{
		m_sphereList.centerX.push_back(0.0f);
		m_sphereList.centerY.push_back(0.0f);
		m_sphereList.centerZ.push_back(0.0f);
		m_sphereList.radiusSq.push_back(0.0f);
		m_sphereList.invRadius.push_back(std::numeric_limits<float>::quiet_NaN());
		m_sphereList.id.push_back(0xFFFFFFFF); // TODO: Make this a constant somewhere
	}

	m_dirty = false;
}