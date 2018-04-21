#pragma once

#include "IMaterial.h"


class Lambertian : public IMaterial
{
public:
	Lambertian() = default;
	explicit Lambertian(Math::Vector3 albedo)
		: m_albedo(albedo)
	{}

	bool Scatter(const Ray& ray, const Hit& hit, Math::Vector3& attenuation, Ray& scattered, uint32_t& state) const override;

	static std::shared_ptr<IMaterial> Make(Math::Vector3 m_albedo);

private:
	Math::Vector3 m_albedo{ Math::kZero };
};