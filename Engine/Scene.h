//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "IAccelerator.h"
#include "IPrimitive.h"


// Forward declarations
class SphereAccelerator;


class Scene : public IPrimitive
{
public:
	
	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

	// Spheres
	void AddSphere(const Math::Vector3& center, float radius, uint32_t id);

private:
	SphereAccelerator * GetSphereAccelerator();

private:
	std::vector<std::shared_ptr<IPrimitive>> m_primList;
	std::vector<std::unique_ptr<IAccelerator>> m_accelList;
};