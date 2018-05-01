//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "IAccelerator.h"


// Forward declarations
class SphereAccelerator;


class Scene
{
public:
	
	bool Intersect(Ray& ray, Hit& hit) const ;
	void Commit();
	
	// Spheres
	void AddSphere(const Math::Vector3& center, float radius, uint32_t id);
	
private:
	SphereAccelerator * GetSphereAccelerator();

private:
	std::vector<std::unique_ptr<IAccelerator>> m_accelList;
};