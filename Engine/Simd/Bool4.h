//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

struct Bool4
{
	// Constructors
	__forceinline Bool4() = default;
	__forceinline Bool4(const Bool4& other)	: v(other.v) {}
	__forceinline Bool4(__m128 input) : v(input) {}

	__forceinline Bool4(bool a)
		: v(g_mmLookupMaskPs[(int(a) << 3) | (int(a) << 2) | (int(a) << 1) | int(a)])
	{}

	__forceinline Bool4(bool a, bool b)
		: v(g_mmLookupMaskPs[(int(b) << 3) | (int(a) << 2) | (int(b) << 1) | int(a)])
	{}

	__forceinline Bool4(bool a, bool b, bool c, bool d)
		: v(g_mmLookupMaskPs[(int(d) << 3) | (int(c) << 2) | (int(b) << 1) | int(a)])
	{}

	__forceinline Bool4(int mask)
	{
		assert(mask >= 0 && mask < 16);
		v = g_mmLookupMaskPs[mask];
	}

	__forceinline Bool4(uint32_t mask)
	{
		assert(mask < 16);
		v = g_mmLookupMaskPs[mask];
	}


	// Assignment operator
	__forceinline Bool4& operator=(const Bool4& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m128&() const { return v; }
	__forceinline operator const __m128i&() const {	return _mm_castps_si128(v);	}


	// Array access operators
	__forceinline bool operator[](size_t index) const
	{
		assert(index < 4);
		return (_mm_movemask_ps(v) >> index) & 1;
	}

	__forceinline int& operator[](size_t index)
	{
		assert(index < 4);
		return i[index];
	}


	static const size_t size = 4;
	union 
	{
		__m128 v;
		int i[4];
	};
};


// Unary operators
__forceinline Bool4 operator!(const Bool4& a) { return _mm_xor_ps(a, Bool4(false)); }


// Binary operators
__forceinline Bool4 operator&(const Bool4& a, const Bool4& b) { return _mm_and_ps(a, b); }
__forceinline Bool4 operator|(const Bool4& a, const Bool4& b) {	return _mm_or_ps(a, b); }
__forceinline Bool4 operator^(const Bool4& a, const Bool4& b) {	return _mm_xor_ps(a, b); }


// Assignment operators
__forceinline Bool4 operator&=(Bool4& a, const Bool4& b) { return a = a & b; }
__forceinline Bool4 operator|=(Bool4& a, const Bool4& b) { return a = a | b; }
__forceinline Bool4 operator^=(Bool4& a, const Bool4& b) { return a = a ^ b; }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline Bool4 Shuffle(const Bool4& a)
{
	return _mm_castsi128_ps(_mm_shuffle_epi32(a, _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i0, int i1, int i2, int i3>
__forceinline Bool4 Shuffle(const Bool4& a, const Bool4& b)
{
	return _mm_shuffle_ps(a, b, _MM_SHUFFLE(i3, i2, i1, i0));
}


// Any/all/none/mask methods
__forceinline bool All(const Bool4& a) { return _mm_movemask_ps(a) == 0xf; }
__forceinline bool Any(const Bool4& a) { return _mm_movemask_ps(a) != 0x0; }
__forceinline bool None(const Bool4& a) { return _mm_movemask_ps(a) == 0x0; }
__forceinline uint32_t Mask(const Bool4& a) { return _mm_movemask_ps(a); }