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
	PrimitiveType GetPrimitiveType() const final
	{
		return PrimitiveType::Sphere;
	}

	void AddSphere(const Math::Vector3& center, float radius, uint32_t id);

	// Intersection methods
	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

private:
	bool IntersectSingleSphere(size_t index, const Ray& ray, float tMin, float tMax, Hit& hit) const;

private:
	std::vector<float>		m_centerX;
	std::vector<float>		m_centerY;
	std::vector<float>		m_centerZ;
	std::vector<float>		m_radiusSq;
	std::vector<float>		m_invRadius;
	std::vector<uint32_t>	m_id;
};