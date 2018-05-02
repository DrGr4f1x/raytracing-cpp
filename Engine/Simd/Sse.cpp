//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#include "stdafx.h"

#include "Sse.h"

const __m128 g_mmLookupMaskPs[16] = 
{
	simd_cast<__m128>(_mm_set_epi32(0, 0, 0, 0)),
	simd_cast<__m128>(_mm_set_epi32(0, 0, 0,-1)),
	simd_cast<__m128>(_mm_set_epi32(0, 0,-1, 0)),
	simd_cast<__m128>(_mm_set_epi32(0, 0,-1,-1)),
	simd_cast<__m128>(_mm_set_epi32(0,-1, 0, 0)),
	simd_cast<__m128>(_mm_set_epi32(0,-1, 0,-1)),
	simd_cast<__m128>(_mm_set_epi32(0,-1,-1, 0)),
	simd_cast<__m128>(_mm_set_epi32(0,-1,-1,-1)),
	simd_cast<__m128>(_mm_set_epi32(-1, 0, 0, 0)),
	simd_cast<__m128>(_mm_set_epi32(-1, 0, 0,-1)),
	simd_cast<__m128>(_mm_set_epi32(-1, 0,-1, 0)),
	simd_cast<__m128>(_mm_set_epi32(-1, 0,-1,-1)),
	simd_cast<__m128>(_mm_set_epi32(-1,-1, 0, 0)),
	simd_cast<__m128>(_mm_set_epi32(-1,-1, 0,-1)),
	simd_cast<__m128>(_mm_set_epi32(-1,-1,-1, 0)),
	simd_cast<__m128>(_mm_set_epi32(-1,-1,-1,-1))
};

const __m128d g_mmLookupMaskPd[4] = 
{
	simd_cast<__m128d>(_mm_set_epi32(0, 0, 0, 0)),
	simd_cast<__m128d>(_mm_set_epi32(0, 0,-1,-1)),
	simd_cast<__m128d>(_mm_set_epi32(-1,-1, 0, 0)),
	simd_cast<__m128d>(_mm_set_epi32(-1,-1,-1,-1))
};