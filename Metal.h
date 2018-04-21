#pragma once

#include "IMaterial.h"

class Metal : public IMaterial
{
public:
	Metal() = default;
	Metal(Math::Vector3 albedo, float fuzz)
		: m_albedo(albedo)
		, m_fuzz(fuzz)
	{}

	bool Scatter(const Ray& ray, const Hit& hit, Math::Vector3& attenuation, Ray& scattered, uint32_t& state) const override;

	static std::shared_ptr<IMaterial> Make(Math::Vector3 m_albedo, float fuzz);

private:
	Math::Vector3 m_albedo{ Math::kZero };
	float m_fuzz{ 0.5f };
};