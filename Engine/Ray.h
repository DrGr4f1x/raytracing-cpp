//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "Math\Vector.h"


struct Ray
{
public:
	Ray() = default;
	Ray(const Math::Vector3& org, const Math::Vector3& dir, float tmin, float tmax)
		: org(org)
		, dir(dir)
		, tmin(tmin)
		, tmax(tmax)
	{}

	Math::Vector3 Eval(float t) const
	{
		return org + t * dir;
	}

	Math::Vector3 org;
	Math::Vector3 dir;
	float tmin;
	float tmax;
};


struct Hit
{
	Math::Vector3 pos;
	Math::Vector3 normal;
	uint32_t geomId;
};