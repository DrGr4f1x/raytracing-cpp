//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

template <int N>
struct Bool
{
	bool n[N];
};

template <int N>
struct Int
{
	int i[N];
};

template <int N>
struct UInt
{
	uint32_t u[N];
};

template <int N>
struct Float
{
	float f[N];
};

// SSE template specializations
using Bool4 =	Bool<4>;
using Int4 =	Int<4>;
using UInt4 =	UInt<4>;
using Float4 =	Float<4>;

// AVX template specializations
using Bool8 =	Bool<8>;
using Int8 =	Int<8>;
using UInt8 =	UInt<8>;
using Float8 =	Float<8>;


#if USE_SSE4
#include "Sse.h"
#endif

#if USE_AVX
#include "Avx.h"
#endif
