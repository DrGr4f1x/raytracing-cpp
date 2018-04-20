//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 
//

#include "stdafx.h"

#include "Random.h"

namespace Math
{
    RandomNumberGenerator g_RNG;

	uint32_t RandomNumberGenerator::SetSeedPIDTime()
	{
		uint32_t seed = 0;
		seed += GetCurrentProcessId();
		seed += static_cast<uint32_t>(std::time(nullptr));

		SetSeed(seed);

		return seed;
	}
}
