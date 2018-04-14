//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "IPrimitive.h"

class Scene : public IPrimitive
{
public:
	void AddPrimitive(IPrimitive* primitive);

	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

private:
	std::vector<IPrimitive*> m_primList;
};