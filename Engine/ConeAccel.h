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
class Scene;


struct ConeList
{
	std::vector<float, aligned_allocator<float, 16>>		centerX;
	std::vector<float, aligned_allocator<float, 16>>		centerY;
	std::vector<float, aligned_allocator<float, 16>>		centerZ;
	std::vector<float, aligned_allocator<float, 16>>		radius;
	std::vector<float, aligned_allocator<float, 16>>		height;
	std::vector<uint32_t, aligned_allocator<uint32_t, 16>>	id;

	__forceinline size_t GetNumCones() const
	{
		return centerX.size();
	}

	__forceinline Math::Vector3 Center(size_t index) const
	{
		return Math::Vector3(centerX[index], centerY[index], centerZ[index]);
	}
};


class ConeAccelerator : public IAccelerator
{
public:
	ConeAccelerator(Scene* scene);

	PrimitiveType GetPrimitiveType() const final
	{
		return PrimitiveType::Cone;
	}

	void AddCone(const Math::Vector3& center, float radius, float height, uint32_t id);

	// Intersection methods
	void Intersect1(Ray& ray, Hit& hit) const final;

	void Commit() final;

private:
	Scene * m_scene;
	ConeList		m_coneList;

	bool			m_dirty{ false };
};