//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "IAccelerator.h"

struct SphereList
{
	std::vector<float, aligned_allocator<float, 16>>		centerX;
	std::vector<float, aligned_allocator<float, 16>>		centerY;
	std::vector<float, aligned_allocator<float, 16>>		centerZ;
	std::vector<float, aligned_allocator<float, 16>>		radiusSq;
	std::vector<float, aligned_allocator<float, 16>>		invRadius;
	std::vector<uint32_t, aligned_allocator<uint32_t, 16>>	id;
};

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
	bool Intersect(Ray& ray, Hit& hit) const final;

	void Commit() final;	

private:
	SphereList		m_sphereList;

	size_t			m_simdSize{ 1 };
	bool			m_dirty{ false };
};