//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "IAccelerator.h"

class SphereAccelerator : public IAccelerator
{
public:
	SphereAccelerator();

	PrimitiveType GetPrimitiveType() const final
	{
		return PrimitiveType::Sphere;
	}

	void AddSphere(const Math::Vector3& center, float radius, uint32_t id);

	// Intersection methods
	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

	void Commit() final;

private:
	// Top-level routines for intersecting rays against our set of spheres
	bool IntersectSpheres1(const Ray& ray, float tMin, float tMax, Hit& hit) const;
	bool IntersectSpheres4(const Ray& ray, float tMin, float tMax, Hit& hit) const;
	bool IntersectSpheres8(const Ray& ray, float tMin, float tMax, Hit& hit) const;

	// Bottom-level routines for intersecting rays against groups of spheres in SIMD
	bool IntersectSphere1(size_t index, const Ray& ray, float tMin, float tMax, Hit& hit) const;
	

private:
	std::vector<float>		m_centerX;
	std::vector<float>		m_centerY;
	std::vector<float>		m_centerZ;
	std::vector<float>		m_radiusSq;
	std::vector<float>		m_invRadius;
	std::vector<uint32_t>	m_id;

	size_t					m_simdSize{ 1 };
	bool					m_dirty{ false };
};