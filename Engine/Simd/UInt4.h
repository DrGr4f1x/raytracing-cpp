//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

struct UInt4
{
	// Constructors
	__forceinline UInt4() = default;
	__forceinline UInt4(const UInt4& other) : v(other.v) {}
	__forceinline UInt4(__m128i v) : v(v) {}
	__forceinline UInt4(uint32_t a) : v(_mm_set1_epi32(a)) {}
	__forceinline UInt4(uint32_t a, uint32_t b, uint32_t c, uint32_t d) : v(_mm_set_epi32(d, c, b, a)) {}
	__forceinline explicit UInt4(const Bool4& a) : v(_mm_castps_si128(a)) {}


	// Assignment operator
	UInt4& operator=(const UInt4& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m128i&() const { return v; }
	__forceinline operator __m128i&() { return v; }


	// Load/store methods
	static __forceinline UInt4 Load(const void* ptr)
	{
		return _mm_load_si128((__m128i*)ptr);
	}

	static __forceinline UInt4 LoadU(const void* ptr)
	{
		return _mm_loadu_si128((__m128i*)ptr);
	}

	static __forceinline void Store(void* ptr, const UInt4& a)
	{
		_mm_store_si128((__m128i*)ptr, a);
	}

	static __forceinline void StoreU(void* ptr, const UInt4& a)
	{
		_mm_storeu_si128((__m128i*)ptr, a);
	}


	// Array access
	__forceinline const uint32_t& operator[](size_t index) const
	{
		assert(index < 4);
		return u[index];
	}
	__forceinline uint32_t& operator[](size_t index)
	{
		assert(index < 4);
		return u[index];
	}


	static const size_t size = 4;
	union 
	{
		__m128i v;
		uint32_t u[4];
	};
};


// Unary operators
__forceinline UInt4 operator+(const UInt4& a) { return a; }
__forceinline UInt4 operator-(const UInt4& a) { return _mm_sub_epi32(_mm_setzero_si128(), a); }


// Binary operators
__forceinline UInt4 operator+(const UInt4& a, const UInt4& b) { return _mm_add_epi32(a, b); }
__forceinline UInt4 operator+(const UInt4& a, uint32_t b) { return a + UInt4(b); }
__forceinline UInt4 operator+(uint32_t a, const UInt4& b) { return UInt4(a) + b; }

__forceinline UInt4 operator-(const UInt4& a, const UInt4& b) { return _mm_sub_epi32(a, b); }
__forceinline UInt4 operator-(const UInt4& a, uint32_t b) { return a - UInt4(b); }
__forceinline UInt4 operator-(uint32_t a, const UInt4& b) { return UInt4(a) - b; }

__forceinline UInt4 operator&(const UInt4& a, const UInt4& b) { return _mm_and_si128(a, b); }
__forceinline UInt4 operator&(const UInt4& a, uint32_t b) { return a & UInt4(b); }
__forceinline UInt4 operator&(uint32_t a, const UInt4& b) { return UInt4(a) & b; }

__forceinline UInt4 operator|(const UInt4& a, const UInt4& b) { return _mm_or_si128(a, b); }
__forceinline UInt4 operator|(const UInt4& a, uint32_t b) { return a | UInt4(b); }
__forceinline UInt4 operator|(uint32_t a, const UInt4& b) { return UInt4(a) | b; }

__forceinline UInt4 operator^(const UInt4& a, const UInt4& b) { return _mm_xor_si128(a, b); }
__forceinline UInt4 operator^(const UInt4& a, uint32_t b) { return a ^ UInt4(b); }
__forceinline UInt4 operator^(uint32_t a, const UInt4& b) { return UInt4(a) ^ b; }

__forceinline UInt4 operator<<(const UInt4& a, uint32_t n) { return _mm_slli_epi32(a, n); }
__forceinline UInt4 operator>>(const UInt4& a, uint32_t n) { return _mm_srai_epi32(a, n); }


// Assignment operators
__forceinline UInt4& operator+=(UInt4& a, const UInt4& b) { return a = a + b; }
__forceinline UInt4& operator+=(UInt4& a, uint32_t b) { return a = a + b; }

__forceinline UInt4& operator-=(UInt4& a, const UInt4& b) { return a = a - b; }
__forceinline UInt4& operator-=(UInt4& a, uint32_t b) { return a = a - b; }

__forceinline UInt4& operator&=(UInt4& a, const UInt4& b) { return a = a & b; }
__forceinline UInt4& operator&=(UInt4& a, uint32_t b) { return a = a & b; }

__forceinline UInt4& operator|=(UInt4& a, const UInt4& b) { return a = a | b; }
__forceinline UInt4& operator|=(UInt4& a, uint32_t b) { return a = a | b; }

__forceinline UInt4& operator^=(UInt4& a, const UInt4& b) { return a = a ^ b; }
__forceinline UInt4& operator^=(UInt4& a, uint32_t b) { return a = a ^ b; }

__forceinline UInt4& operator<<=(UInt4& a, uint32_t n) { return a = a << n; }
__forceinline UInt4& operator>>=(UInt4& a, uint32_t n) { return a = a >> n; }


// Comparison operators
__forceinline Bool4 operator==(const UInt4& a, const UInt4& b) { return _mm_castsi128_ps(_mm_cmpeq_epi32(a, b)); }
__forceinline Bool4 operator!=(const UInt4& a, const UInt4& b) { return !(a == b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline UInt4 Shuffle(const UInt4& a)
{
	return _mm_shuffle_epi32(a, _MM_SHUFFLE(i3, i2, i1, i0));
}

template <int i0, int i1, int i2, int i3>
__forceinline UInt4 Shuffle(const UInt4& a, const UInt4& b)
{
	return _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i>
__forceinline UInt4 Shuffle(const UInt4& a)
{
	return Shuffle<i, i, i, i>(a);
}

template <int i0, int i1, int i2, int i3>
__forceinline UInt4 Shuffle(uint32_t a)
{
	return Shuffle<i0, i1, i2, i3>(UInt4(a));
}

template <int i0, int i1, int i2, int i3>
__forceinline UInt4 Shuffle(uint32_t a, uint32_t b)
{
	return Shuffle<i0, i1, i2, i3>(UInt4(a), UInt4(b));
}

template <int i>
__forceinline UInt4 Shuffle(uint32_t a)
{
	return Shuffle<i, i, i, i>(UInt4(a));
}


// Misc math methods
__forceinline UInt4 Select(const Bool4& m, const UInt4& t, const UInt4& f)
{
	return _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(f), _mm_castsi128_ps(t), m));
}