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


// Utilities to make SSE SIMD casts less awful
template <typename T>
__forceinline T simd_cast(__m128 src) {	static_assert(false, "Must use one of the template specializations"); }

template <typename T>
__forceinline T simd_cast(__m128i src) { static_assert(false, "Must use one of the template specializations"); }

template <typename T>
__forceinline T simd_cast(__m128d src) { static_assert(false, "Must use one of the template specializations"); }

template <>
__forceinline __m128i simd_cast<__m128i>(__m128 src) { return _mm_castps_si128(src);  }

template <>
__forceinline __m128d simd_cast<__m128d>(__m128 src) { return _mm_castps_pd(src); }

template <>
__forceinline __m128 simd_cast<__m128>(__m128i src) { return _mm_castsi128_ps(src); }

template <>
__forceinline __m128d simd_cast<__m128d>(__m128i src) { return _mm_castsi128_pd(src); }

template <>
__forceinline __m128 simd_cast<__m128>(__m128d src) { return _mm_castpd_ps(src); }

template <>
__forceinline __m128i simd_cast<__m128i>(__m128d src) { return _mm_castpd_si128(src); }


// Utilities to make AVX SIMD casts less awful
template <typename T>
__forceinline T simd_cast(__m256 src) { static_assert(false, "Must use one of the template specializations"); }

template <typename T>
__forceinline T simd_cast(__m256i src) { static_assert(false, "Must use one of the template specializations"); }

template <typename T>
__forceinline T simd_cast(__m256d src) { static_assert(false, "Must use one of the template specializations"); }

template <>
__forceinline __m256i simd_cast<__m256i>(__m256 src) { return _mm256_castps_si256(src); }

template <>
__forceinline __m256d simd_cast<__m256d>(__m256 src) { return _mm256_castps_pd(src); }

template <>
__forceinline __m256 simd_cast<__m256>(__m256i src) { return _mm256_castsi256_ps(src); }

template <>
__forceinline __m256d simd_cast<__m256d>(__m256i src) { return _mm256_castsi256_pd(src); }

template <>
__forceinline __m256 simd_cast<__m256>(__m256d src) { return _mm256_castpd_ps(src); }

template <>
__forceinline __m256i simd_cast<__m256i>(__m256d src) { return _mm256_castpd_si256(src); }


#if USE_SSE4
#include "Sse.h"
#endif

#if USE_AVX
#include "Avx.h"
#endif
