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
	Sphere(Math::Vector3 center, float radius)
		: m_center(center)
		, m_radius(radius)
	{}

	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

private:
	Math::Vector3	m_center{ 0.0f, 0.0f, 0.0f };
	float			m_radius{ 1.0f };
};