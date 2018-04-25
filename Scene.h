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
	void AddPrimitive(std::shared_ptr<IPrimitive> primitive);
	size_t GetNumPrimitives() const { return m_primList.size(); }
	IPrimitive* GetPrimitive(size_t index)
	{
		return m_primList[index].get();
	}

	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

private:
	std::vector<std::shared_ptr<IPrimitive>> m_primList;
};