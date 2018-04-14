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

using namespace Math;


void Scene::AddPrimitive(IPrimitive* primitive)
{
	m_primList.push_back(primitive);
}


bool Scene::Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const
{
	Hit tempHit;
	bool hitAnything = false;
	float closestHit = tMax;

	for (auto prim : m_primList)
	{
		if (prim->Intersect(ray, tMin, closestHit, tempHit))
		{
			hitAnything = true;
			closestHit = tempHit.t;
			hit = tempHit;
		}
	}

	return hitAnything;
}