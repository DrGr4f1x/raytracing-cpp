//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "MaterialSet.h"

#include "Sampling.h"
#include "Math\Random.h"


using namespace Math;


void MaterialSet::Reserve(size_t numMaterials)
{
	m_albedoList.reserve(numMaterials);
	m_miscFloatList.reserve(numMaterials);
	m_materialTypeList.reserve(numMaterials);
}


size_t MaterialSet::AddLambertian(const Vector3& albedo)
{
	size_t size = m_materialTypeList.size();

	m_albedoList.push_back(albedo);
	m_miscFloatList.push_back(0.0f);
	m_materialTypeList.push_back(MaterialType::Lambertian);

	return size;
}


size_t MaterialSet::AddMetallic(const Vector3& albedo, float fuzz)
{
	size_t size = m_materialTypeList.size();

	m_albedoList.push_back(albedo);
	m_miscFloatList.push_back(fuzz);
	m_materialTypeList.push_back(MaterialType::Metallic);

	return size;
}


size_t MaterialSet::AddDielectric(float refractionIndex)
{
	size_t size = m_materialTypeList.size();

	m_albedoList.push_back(Vector3(kZero));
	m_miscFloatList.push_back(refractionIndex);
	m_materialTypeList.push_back(MaterialType::Dielectric);

	return size;
}


bool Refract(Vector3 v, Vector3 n, float ni_over_nt, Vector3& refracted)
{
	Vector3 uv = Normalize(v);
	float dt = Dot(uv, n);
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
	if (discriminant > 0.0f)
	{
		refracted = ni_over_nt * (uv - dt * n) - n * sqrtf(discriminant);
		return true;
	}

	return false;
}


float Schlick(float cosine, float refractionIdx)
{
	float r0 = (1.0f - refractionIdx) / (1.0f + refractionIdx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}


bool MaterialSet::Scatter(size_t index, const Ray& ray, const Hit& hit, Math::Vector3& attenuation, Ray& scattered, uint32_t& state)
{
	switch (m_materialTypeList[index])
	{
	case MaterialType::Lambertian:
	{
		Vector3 target = hit.pos + hit.normal + UniformUnitSphere3d(state);
		scattered = Ray(hit.pos, Normalize(target - hit.pos), 0.001f, FLT_MAX);
		attenuation = m_albedoList[index];
		return true;
	}
	break;

	case MaterialType::Metallic:
	{
		Vector3 reflected = Reflect(Normalize(ray.dir), hit.normal);
		scattered = Ray(hit.pos, Normalize(reflected + m_miscFloatList[index] * UniformUnitSphere3d(state)), 0.001f, FLT_MAX);
		attenuation = m_albedoList[index];
		return (Dot(scattered.dir, hit.normal) > 0.0f);
	}
	break;

	case MaterialType::Dielectric:
	{
		Vector3 outNormal{ kZero };
		Vector3 reflected = Reflect(ray.dir, hit.normal);
		float ni_over_nt = 0.0f;
		attenuation = Vector3{ kOne };
		Vector3 refracted{ kZero };
		float reflectProb = 1.0f;
		float cosine = 0.0f;
		float refractionIndex = m_miscFloatList[index];
		if (Dot(ray.dir, hit.normal) > 0.0f)
		{
			outNormal = -hit.normal;
			ni_over_nt = refractionIndex;
			cosine = refractionIndex * Dot(ray.dir, hit.normal) / Length(ray.dir);
		}
		else
		{
			outNormal = hit.normal;
			ni_over_nt = 1.0f / refractionIndex;
			cosine = -Dot(ray.dir, hit.normal) / Length(ray.dir);
		}

		if (Refract(ray.dir, outNormal, ni_over_nt, refracted))
		{
			reflectProb = Schlick(cosine, refractionIndex);
		}
		else
		{
			scattered = Ray(hit.pos, Normalize(reflected), 0.001f, FLT_MAX);
		}

		if (UniformFloat01(state) < reflectProb)
		{
			scattered = Ray(hit.pos, Normalize(reflected), 0.001f, FLT_MAX);
		}
		else
		{
			scattered = Ray(hit.pos, Normalize(refracted), 0.001f, FLT_MAX);
		}

		return true;
	}
	break;
	}

	return false;
}