#pragma once

class Camera
{
public:
	Camera() = default;

	void LookAt(Math::Vector3 pos, Math::Vector3 target, Math::Vector3 up, float fovY, float aspect);

	Ray GetRay(float u, float v) const;

private:
	Math::Vector3 m_origin;
	Math::Vector3 m_lowerLeft;
	Math::Vector3 m_horizontal;
	Math::Vector3 m_vertical;
};