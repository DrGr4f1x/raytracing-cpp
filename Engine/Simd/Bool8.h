//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

template <>
struct Bool<8>
{
	// Constructors
	__forceinline Bool() = default;
	__forceinline Bool(const Bool& other) : v(other.v) {}
	__forceinline Bool(__m256 input) : v(input) {}

	__forceinline Bool(const Bool4& a) : v(_mm256_insertf128_ps(_mm256_castps128_ps256(a), a, 1)) {}
	__forceinline Bool(const Bool4& a, const Bool4& b) : v(_mm256_insertf128_ps(_mm256_castps128_ps256(a), b, 1)) {}
	__forceinline Bool(__m128 a, __m128 b) : low(a), high(b) {}

	__forceinline Bool(bool a) : v(Bool(Bool4(a), Bool4(a))) {}
	__forceinline Bool(bool a, bool b) : v(Bool(Bool4(a), Bool4(b))) {}
	__forceinline Bool(bool a, bool b, bool c, bool d) 
		: v(Bool(Bool4(a, b), Bool4(c, d))) 
	{}
	__forceinline Bool(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h) 
		: v(Bool(Bool4(a, b, c, d), Bool4(e, f, g, h))) 
	{}

	__forceinline Bool(int mask)
	{
		assert(mask >= 0 && mask < 256);
		low = g_mmLookupMaskPs[mask & 0xf];
		high = g_mmLookupMaskPs[mask >> 4];
	}

	// Assignment operator
	__forceinline Bool& operator=(const Bool& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m256&() const { return v; }
	__forceinline operator const __m256i&() const { return simd_cast<__m256i>(v); }


	// Array access operators
	__forceinline bool operator[](size_t index) const
	{
		assert(index < 8);
		return (_mm256_movemask_ps(v) >> index) & 1;
	}

	__forceinline int& operator[](size_t index)
	{
		assert(index < 8);
		return i[index];
	}


	static const size_t size = 8;
	union
	{
		__m256 v;
		struct  
		{
			__m128 low;
			__m128 high;
		};
		int i[8];
	};
};


// Unary operators
__forceinline Bool8 operator!(const Bool8& a) { return _mm256_xor_ps(a, Bool8(false)); }


// Binary operators
__forceinline Bool8 operator&(const Bool8& a, const Bool8& b) { return _mm256_and_ps(a, b); }
__forceinline Bool8 operator|(const Bool8& a, const Bool8& b) { return _mm256_or_ps(a, b); }
__forceinline Bool8 operator^(const Bool8& a, const Bool8& b) { return _mm256_xor_ps(a, b); }


// Assignment operators
__forceinline Bool8 operator&=(Bool8& a, const Bool8& b) { return a = a & b; }
__forceinline Bool8 operator|=(Bool8& a, const Bool8& b) { return a = a | b; }
__forceinline Bool8 operator^=(Bool8& a, const Bool8& b) { return a = a ^ b; }


// Shuffle methods
template <int i>
__forceinline Bool8 Shuffle(const Bool8& a)
{
	return _mm256_permute_ps(a, _MM_SHUFFLE(i, i, i, i));
}

template <int i0, int i1, int i2, int i3>
__forceinline Bool8 Shuffle(const Bool8& a)
{
	return _mm256_permute_ps(a, _MM_SHUFFLE(i3, i2, i1, i0));
}

template <int i0, int i1, int i2, int i3>
__forceinline Bool8 Shuffle(const Bool8& a, const Bool8& b)
{
	return _mm256_shuffle_ps(a, b, _MM_SHUFFLE(i3, i2, i1, i0));
}


// Any/all/none/mask methods
__forceinline bool All(const Bool8& a) { return _mm256_movemask_ps(a) == 0xff; }
__forceinline bool Any(const Bool8& a) { return !_mm256_testz_ps(a, a); }
__forceinline bool None(const Bool8& a) { return _mm256_testz_ps(a, a) != 0; }
__forceinline uint32_t Mask(const Bool8& a) { return _mm256_movemask_ps(a); }