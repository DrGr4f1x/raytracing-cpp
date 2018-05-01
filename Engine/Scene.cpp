//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "Scene.h"

#include "Ray.h"
#include "SphereAccel.h"


using namespace std;
using namespace Math;


bool Scene::Intersect(Ray& ray, Hit& hit) const
{
	for (auto& p : m_accelList)
	{
		if (p->Intersect(ray, hit))
		{
			return true;
		}
	}
	return false;
}


void Scene::Commit()
{
	for (auto& p : m_accelList)
	{
		p->Commit();
	}
}


void Scene::AddSphere(const Vector3& center, float radius, uint32_t id)
{
	GetSphereAccelerator()->AddSphere(center, radius, id);
}


SphereAccelerator* Scene::GetSphereAccelerator()
{
	SphereAccelerator* accel = nullptr;
	for (auto& p : m_accelList)
	{
		if (p->GetPrimitiveType() == PrimitiveType::Sphere)
		{
			accel = (SphereAccelerator*)p.get();
			break;
		}
	}

	if (!accel)
	{
		auto newAccel = make_unique<SphereAccelerator>();
		accel = newAccel.get();
		m_accelList.emplace_back(move(newAccel));
	}

	return accel;
}