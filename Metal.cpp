#include "stdafx.h"

#include "Metal.h"

#include "Sampling.h"
#include "Math\Random.h"


using namespace std;
using namespace Math;


bool Metal::Scatter(const Ray& ray, const Hit& hit, Vector3& attenuation, Ray& scattered, RandomNumberGenerator& rng) const
{
	Vector3 reflected = Reflect(Normalize(ray.Direction()), hit.normal);
	scattered = Ray(hit.pos, reflected + m_fuzz * UniformUnitSphere3d(rng));
	attenuation = m_albedo;
	return (Dot(scattered.Direction(), hit.normal) > 0.0f);
}


shared_ptr<IMaterial> Metal::Make(Vector3 albedo, float fuzz)
{
	return make_shared<Metal>(albedo, fuzz);
}