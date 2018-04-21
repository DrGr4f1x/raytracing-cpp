#include "stdafx.h"

#include "Lambertian.h"

#include "Sampling.h"
#include "Math\Random.h"


using namespace std;
using namespace Math;


bool Lambertian::Scatter(const Ray& ray, const Hit& hit, Vector3& attenuation, Ray& scattered, uint32_t& state) const
{
	Vector3 target = hit.pos + hit.normal + UniformUnitSphere3d(state);
	scattered = Ray(hit.pos, target - hit.pos);
	attenuation = m_albedo;
	return true;
}


shared_ptr<IMaterial> Lambertian::Make(Vector3 albedo)
{
	return make_shared<Lambertian>(albedo);
}