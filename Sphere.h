//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "IPrimitive.h"


class Sphere : public IPrimitive
{
public:
	Sphere() = default;
	Sphere(Math::Vector3 center, float radius, uint32_t geomId)
		: m_center(center)
		, m_radius(radius)
		, m_geomId(geomId)
	{}



	static std::shared_ptr<IPrimitive> Make(Math::Vector3 center, float radius);

	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

	Math::Vector3 GetCenter() const { return m_center; }
	float GetCenterX() const { return m_center.GetX(); }
	float GetCenterY() const { return m_center.GetY(); }
	float GetCenterZ() const { return m_center.GetZ(); }
	float GetRadius() const { return m_radius; }
	float GetRadiusSq() const { return m_radius * m_radius; }

private:
	Math::Vector3	m_center{ 0.0f, 0.0f, 0.0f };
	float			m_radius{ 1.0f };
	uint32_t		m_geomId{ 0 };
};