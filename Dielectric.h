#pragma once

#include "IMaterial.h"

class Dielectric : public IMaterial
{
public:
	Dielectric() = default;
	Dielectric(float refractionIdx)
		: m_refractionIdx(refractionIdx)
	{}

	bool Scatter(const Ray& ray, const Hit& hit, Math::Vector3& attenuation, Ray& scattered, Math::RandomNumberGenerator& rng) const override;

	static std::shared_ptr<IMaterial> Make(float refractionIdx);

private:
	float m_refractionIdx{ 0.5f };
};