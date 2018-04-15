//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "IMaterial.h"
#include "IPrimitive.h"


class Sphere : public IPrimitive
{
public:
	Sphere() = default;
	Sphere(Math::Vector3 center, float radius)
		: m_center(center)
		, m_radius(radius)
	{}

	static std::shared_ptr<IPrimitive> Make(Math::Vector3 center, float radius);

	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

	void SetMaterial(std::shared_ptr<IMaterial> material) final
	{
		m_material = material;
	}
	const IMaterial* GetMaterial() const final { return m_material.get(); }

private:
	Math::Vector3	m_center{ 0.0f, 0.0f, 0.0f };
	float			m_radius{ 1.0f };
	std::shared_ptr<IMaterial> m_material;
};