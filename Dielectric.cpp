#include "stdafx.h"

#include "Dielectric.h"

#include "Sampling.h"
#include "Math\Random.h"


using namespace std;
using namespace Math;


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


bool Dielectric::Scatter(const Ray& ray, const Hit& hit, Vector3& attenuation, Ray& scattered, RandomNumberGenerator& rng) const
{
	Vector3 outNormal{ kZero };
	Vector3 reflected = Reflect(ray.Direction(), hit.normal);
	float ni_over_nt = 0.0f;
	attenuation = Vector3{ kOne };
	Vector3 refracted{ kZero };
	float reflectProb = 1.0f;
	float cosine = 0.0f;
	if(Dot(ray.Direction(), hit.normal) > 0.0f)
	{ 
		outNormal = -hit.normal;
		ni_over_nt = m_refractionIdx;
		cosine = m_refractionIdx * Dot(ray.Direction(), hit.normal) / Length(ray.Direction());
	}
	else
	{
		outNormal = hit.normal;
		ni_over_nt = 1.0f / m_refractionIdx;
		cosine = -Dot(ray.Direction(), hit.normal) / Length(ray.Direction());
	}

	if (Refract(ray.Direction(), outNormal, ni_over_nt, refracted))
	{
		reflectProb = Schlick(cosine, m_refractionIdx);
	}
	else
	{
		scattered = Ray(hit.pos, reflected);
	}

	if (rng.NextFloat() < reflectProb)
	{
		scattered = Ray(hit.pos, reflected);
	}
	else
	{
		scattered = Ray(hit.pos, refracted);
	}

	return true;
}


shared_ptr<IMaterial> Dielectric::Make(float refractionIdx)
{
	return make_shared<Dielectric>(refractionIdx);
}