#include "stdafx.h"

#include "Camera.h"


using namespace Math;


void Camera::LookAt(Vector3 pos, Vector3 target, Vector3 up, float fovY, float aspect)
{
	float theta = DirectX::XMConvertToRadians(fovY);
	float halfHeight = tanf(0.5f * theta);
	float halfWidth = aspect * halfHeight;

	m_origin = pos;

	Vector3 w = Normalize(pos - target);
	Vector3 u = Normalize(Cross(up, w));
	Vector3 v = Cross(w, u);

	m_lowerLeft = m_origin - halfWidth * u - halfHeight * v - w;
	m_horizontal = 2.0f * halfWidth * u;
	m_vertical = 2.0f * halfHeight * v;
}


Ray Camera::GetRay(float u, float v) const
{
	return Ray(m_origin, m_lowerLeft + u * m_horizontal + v * m_vertical - m_origin);
}