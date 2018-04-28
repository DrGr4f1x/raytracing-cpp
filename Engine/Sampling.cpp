#include "stdafx.h"

#include "Sampling.h"

#include "Math\Random.h"


using namespace Math;


static uint32_t XorShift32(uint32_t& state)
{
	uint32_t x = state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 15;
	state = x;
	return x;
}


float UniformFloat01(uint32_t& state)
{
	return (XorShift32(state) & 0xFFFFFF) / 16777216.0f;
}


Vector3 UniformUnitSphere3d(uint32_t& state)
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
		p = 2.0f * Vector3(UniformFloat01(state), UniformFloat01(state), UniformFloat01(state)) - Vector3(kOne);
	} while (LengthSquare(p) >= 1.0f);
	return p;
}


Vector3 UniformUnitDisk(uint32_t& state)
{
	/*float r = rng.NextFloat();
	float phi = rng.NextFloat(XM_2PI);

	r = sqrtf(r);

	return Vector3(r * cosf(phi), r * sinf(phi), 0.0f);*/

	Vector3 p;
	do {
		p = 2.0f * Vector3(UniformFloat01(state), UniformFloat01(state), 0.0f) - Vector3(1.0f, 1.0f, 0);
	} while (Dot(p, p) >= 1.0f);
	return p;
}