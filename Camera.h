#pragma once

namespace Math
{
class RandomNumberGenerator;
}

class Camera
{
public:
	Camera() = default;

	void LookAt(Math::Vector3 pos, Math::Vector3 target, Math::Vector3 up, float fovY, float aspect, float aperture, float focusDist);

	Ray GetRay(float u, float v, Math::RandomNumberGenerator& rng) const;

private:
	Math::Vector3 m_origin;
	Math::Vector3 m_lowerLeft;
	Math::Vector3 m_horizontal;
	Math::Vector3 m_vertical;
	Math::Vector3 m_u;
	Math::Vector3 m_v;
	Math::Vector3 m_w;
	float m_lensRadius;
};