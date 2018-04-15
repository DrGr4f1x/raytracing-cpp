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

class Scene : public IPrimitive
{
public:
	void AddPrimitive(std::shared_ptr<IPrimitive> primitive);

	bool Intersect(const Ray& ray, float tMin, float tMax, Hit& hit) const final;

	void SetMaterial(std::shared_ptr<IMaterial> material) final {}
	const IMaterial* GetMaterial() const final { return nullptr; }

private:
	std::vector<std::shared_ptr<IPrimitive>> m_primList;
};