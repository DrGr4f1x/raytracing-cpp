//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

enum class MaterialType
{
	Lambertian,
	Metallic,
	Dielectric
};

class MaterialSet
{
public:
	void Reserve(size_t numMaterials);

	size_t AddLambertian(const Math::Vector3& albedo);
	size_t AddMetallic(const Math::Vector3& albedo, float fuzz);
	size_t AddDielectric(float refractionIndex);

	bool Scatter(const Ray& ray, const Hit& hit, Math::Vector3& attenuation, Ray& scattered, uint32_t& state);

private:
	std::vector<Math::Vector3>	m_albedoList;
	std::vector<float>			m_miscFloatList;
	std::vector<MaterialType>	m_materialTypeList;
};