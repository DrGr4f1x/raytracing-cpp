//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#include "Math\Vector.h"

__declspec(align(16)) struct Ray
{
	float posX;
	float posY;
	float posZ;
	float tmin;

	float dirX;
	float dirY;
	float dirZ;
	float tmax;
};


__declspec(align(16)) struct Hit
{
	float normalX;
	float normalY;
	float normalZ;
	uint32_t geomId;
};