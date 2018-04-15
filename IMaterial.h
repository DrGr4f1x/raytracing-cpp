#pragma once

namespace Math
{
class RandomNumberGenerator;
}

class IMaterial
{
public:
	virtual bool Scatter(
		const Ray& ray, 
		const Hit& hit, 
		Math::Vector3& attenuation, 
		Ray& scattered, 
		Math::RandomNumberGenerator& rng) const = 0;
};