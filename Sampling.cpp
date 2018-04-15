#include "stdafx.h"

#include "Sampling.h"

#include "Math\Random.h"


using namespace Math;


Vector3 UniformUnitSphere3d(RandomNumberGenerator& rng)
{
	/*float phi = rng.NextFloat(XM_2PI);
	float cosTheta = rng.NextFloat(-1.0f, 1.0f);
	float u = rng.NextFloat();

	float theta = acosf(cosTheta);
	float r = powf(u, 1.0f / 3.0f);

	float x = r * sinf(theta) * cosf(phi);
	float y = r * sinf(theta) * sinf(phi);
	float z = r * cosf(theta);

	return Vector3(x, y, z);*/

	Vector3 p;
	do {
		p = 2.0f * Vector3(rng.NextFloat(), rng.NextFloat(), rng.NextFloat()) - Vector3(1.0f, 1.0f, 1.0f);
	} while (LengthSquare(p) >= 1.0f);
	return p;
}


Vector3 UniformUnitDisk(RandomNumberGenerator& rng)
{
	/*float r = rng.NextFloat();
	float phi = rng.NextFloat(XM_2PI);

	r = sqrtf(r);

	return Vector3(r * cosf(phi), r * sinf(phi), 0.0f);*/

	Vector3 p;
	do {
		p = 2.0f * Vector3(rng.NextFloat(), rng.NextFloat(), 0) - Vector3(1.0f, 1.0f, 0);
	} while (Dot(p, p) >= 1.0f);
	return p;
}