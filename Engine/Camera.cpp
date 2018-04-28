#include "stdafx.h"

#include "Camera.h"

#include "Sampling.h"
#include "Math\Random.h"


using namespace Math;


void Camera::LookAt(Vector3 pos, Vector3 target, Vector3 up, float fovY, float aspect, float aperture, float focusDist)
{
	m_lensRadius = aperture * 0.5f;

	float theta = DirectX::XMConvertToRadians(fovY);
	float halfHeight = tanf(0.5f * theta);
	float halfWidth = aspect * halfHeight;

	m_origin = pos;

	m_w = Normalize(pos - target);
	m_u = Normalize(Cross(up, m_w));
	m_v = Cross(m_w, m_u);

	m_lowerLeft = m_origin - halfWidth * focusDist * m_u - halfHeight * focusDist * m_v - focusDist * m_w;
	m_horizontal = 2.0f * halfWidth * focusDist * m_u;
	m_vertical = 2.0f * halfHeight * focusDist * m_v;
}


Ray Camera::GetRay(float u, float v, uint32_t& state) const
{
	Vector3 rndDisk = m_lensRadius * UniformUnitDisk(state);
	Vector3 offset = m_u * rndDisk.GetX() + m_v * rndDisk.GetY();
	return Ray(m_origin + offset, m_lowerLeft + u * m_horizontal + v * m_vertical - m_origin - offset);
}