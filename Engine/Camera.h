//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once


class Camera
{
public:
	Camera() = default;

	void LookAt(Math::Vector3 pos, Math::Vector3 target, Math::Vector3 up, float fovY, float aspect, float aperture, float focusDist);

	Ray GetRay(float u, float v, uint32_t& rng) const;

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