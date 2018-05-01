//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

#if USE_AVX2

template <>
struct UInt<8>
{
	// Constructors
	__forceinline UInt() = default;
	__forceinline UInt(const UInt& other) : v(other.v) {}
	__forceinline UInt(__m256i v) : v(v) {}
	__forceinline explicit UInt(__m256 v) : v(_mm256_cvtps_epi32(v)) {}
	__forceinline UInt(uint32_t a) : v(_mm256_set1_epi32(a)) {}
	__forceinline UInt(uint32_t a, uint32_t b) : v(_mm256_set_epi32(b, a, b, a, b, a, b, a)) {}
	__forceinline UInt(uint32_t a, uint32_t b, uint32_t c, uint32_t d) 
		: v(_mm256_set_epi32(d, c, b, a, d, c, b, a)) 
	{}
	__forceinline UInt(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h) 
		: v(_mm256_set_epi32(h, g, f, e, d, c, b, a)) 
	{}


	// Assignment operator
	__forceinline UInt& operator=(const UInt& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m256i&() const { return v; }
	__forceinline operator __m256i&() { return v; }


	// Load/store methods
	static __forceinline UInt Load(const void* ptr)
	{
		return _mm256_load_si256((__m256i*)ptr);
	}

	static __forceinline UInt LoadU(const void* ptr)
	{
		return _mm256_loadu_si256((__m256i*)ptr);
	}

	static __forceinline void Store(void* ptr, const UInt& a)
	{
		_mm256_store_si256((__m256i*)ptr, a);
	}

	static __forceinline void StoreU(void* ptr, const UInt& a)
	{
		_mm256_storeu_si256((__m256i*)ptr, a);
	}


	// Array access
	__forceinline const uint32_t& operator[](size_t index) const
	{
		assert(index < 8);
		return u[index];
	}
	__forceinline uint32_t& operator[](size_t index)
	{
		assert(index < 8);
		return u[index];
	}


	static const size_t size = 8;
	union
	{
		__m256i v;
		uint32_t u[8];
	};
};


// Unary operators
__forceinline UInt8 operator+(const UInt8& a) { return a; }
__forceinline UInt8 operator-(const UInt8& a) { return _mm256_sub_epi32(_mm256_setzero_si256(), a); }


// Binary operators
__forceinline UInt8 operator+(const UInt8& a, const UInt8& b) { return _mm256_add_epi32(a, b); }
__forceinline UInt8 operator+(const UInt8& a, uint32_t b) { return a + UInt8(b); }
__forceinline UInt8 operator+(uint32_t a, const UInt8& b) { return UInt8(a) + b; }

__forceinline UInt8 operator-(const UInt8& a, const UInt8& b) { return _mm256_sub_epi32(a, b); }
__forceinline UInt8 operator-(const UInt8& a, uint32_t b) { return a - UInt8(b); }
__forceinline UInt8 operator-(uint32_t a, const UInt8& b) { return UInt8(a) - b; }

__forceinline UInt8 operator&(const UInt8& a, const UInt8& b) { return _mm256_and_si256(a, b); }
__forceinline UInt8 operator&(const UInt8& a, uint32_t b) { return a & UInt8(b); }
__forceinline UInt8 operator&(uint32_t a, const UInt8& b) { return UInt8(a) & b; }

__forceinline UInt8 operator|(const UInt8& a, const UInt8& b) { return _mm256_or_si256(a, b); }
__forceinline UInt8 operator|(const UInt8& a, uint32_t b) { return a | UInt8(b); }
__forceinline UInt8 operator|(uint32_t a, const UInt8& b) { return UInt8(a) | b; }

__forceinline UInt8 operator^(const UInt8& a, const UInt8& b) { return _mm256_xor_si256(a, b); }
__forceinline UInt8 operator^(const UInt8& a, uint32_t b) { return a ^ UInt8(b); }
__forceinline UInt8 operator^(uint32_t a, const UInt8& b) { return UInt8(a) ^ b; }

__forceinline UInt8 operator<<(const UInt8& a, int n) { return _mm256_slli_epi32(a, n); }
__forceinline UInt8 operator>>(const UInt8& a, int n) { return _mm256_srai_epi32(a, n); }


// Assignment operators
__forceinline UInt8& operator+=(UInt8& a, const UInt8& b) { return a = a + b; }
__forceinline UInt8& operator+=(UInt8& a, uint32_t b) { return a = a + b; }

__forceinline UInt8& operator-=(UInt8& a, const UInt8& b) { return a = a - b; }
__forceinline UInt8& operator-=(UInt8& a, uint32_t b) { return a = a - b; }

__forceinline UInt8& operator&=(UInt8& a, const UInt8& b) { return a = a & b; }
__forceinline UInt8& operator&=(UInt8& a, uint32_t b) { return a = a & b; }

__forceinline UInt8& operator|=(UInt8& a, const UInt8& b) { return a = a | b; }
__forceinline UInt8& operator|=(UInt8& a, uint32_t b) { return a = a | b; }

__forceinline UInt8& operator^=(UInt8& a, const UInt8& b) { return a = a ^ b; }
__forceinline UInt8& operator^=(UInt8& a, uint32_t b) { return a = a ^ b; }

__forceinline UInt8& operator<<=(UInt8& a, int n) { return a = a << n; }
__forceinline UInt8& operator>>=(UInt8& a, int n) { return a = a >> n; }


// Comparison operators
__forceinline Bool8 operator==(const UInt8& a, const UInt8& b) { return _mm256_castsi256_ps(_mm256_cmpeq_epi32(a, b)); }
__forceinline Bool8 operator!=(const UInt8& a, const UInt8& b) { return !(a == b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline UInt8 Shuffle(const UInt8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i0, int i1, int i2, int i3>
__forceinline UInt8 Shuffle(const UInt8& a, const UInt8& b)
{
	return _mm256_permute2f128_si256(a, b, (i1 << 4) | (i0 << 0));
}

template <int i>
__forceinline UInt8 Shuffle(const UInt8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i, i, i, i)));
}


// Misc math methods
__forceinline UInt8 Select(const Bool8& m, const UInt8& t, const UInt8& f)
{
	return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(f), _mm256_castsi256_ps(t), m));
}

#else

template <>
struct UInt<8>
{
	// Constructors
	__forceinline UInt() = default;
	__forceinline UInt(const UInt& other) : v(other.v) {}
	__forceinline UInt(__m256i v) : v(v) {}
	__forceinline UInt(const __m128i& a, const __m128i& b) : low(a), high(b) {}
	__forceinline explicit UInt(__m256 v) : v(_mm256_cvtps_epi32(v)) {}
	__forceinline UInt(uint32_t a) : v(_mm256_set1_epi32(a)) {}
	__forceinline UInt(uint32_t a, uint32_t b) : v(_mm256_set_epi32(b, a, b, a, b, a, b, a)) {}
	__forceinline UInt(uint32_t a, uint32_t b, uint32_t c, uint32_t d) 
		: v(_mm256_set_epi32(d, c, b, a, d, c, b, a)) 
	{}
	__forceinline UInt(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h) 
		: v(_mm256_set_epi32(h, g, f, e, d, c, b, a)) 
	{}


	// Assignment operator
	__forceinline UInt& operator=(const UInt& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m256i&() const { return v; }
	__forceinline operator __m256i&() { return v; }


	// Load/store methods
	static __forceinline UInt Load(const void* ptr)
	{
		return _mm256_castps_si256(_mm256_load_ps((float*)ptr));
	}

	static __forceinline UInt LoadU(const void* ptr)
	{
		return  _mm256_castps_si256(_mm256_loadu_ps((float*)ptr));
	}

	static __forceinline void Store(void* ptr, const UInt& a)
	{
		_mm256_store_ps((float*)ptr, _mm256_castsi256_ps(a));
	}

	static __forceinline void StoreU(void* ptr, const UInt& a)
	{
		_mm256_storeu_ps((float*)ptr, _mm256_castsi256_ps(a));
	}


	// Array access
	__forceinline const uint32_t& operator[](size_t index) const
	{
		assert(index < 8);
		return u[index];
	}
	__forceinline uint32_t& operator[](size_t index)
	{
		assert(index < 8);
		return u[index];
	}


	static const size_t size = 8;
	union
	{
		__m256i v;
		struct
		{
			__m128i low;
			__m128i high;
		};
		int u[8];
	};
};


// Unary operators
__forceinline UInt8 operator+(const UInt8& a) { return a; }
__forceinline UInt8 operator-(const UInt8& a) { UInt8(_mm_sub_epi32(_mm_setzero_si128(), a.low), _mm_sub_epi32(_mm_setzero_si128(), a.high)); }


// Binary operators
__forceinline UInt8 operator+(const UInt8& a, const UInt8& b) { return UInt8(_mm_add_epi32(a.low, b.low), _mm_add_epi32(a.high, b.high)); }
__forceinline UInt8 operator+(const UInt8& a, int b) { return a + UInt8(b); }
__forceinline UInt8 operator+(int a, const UInt8& b) { return UInt8(a) + b; }

__forceinline UInt8 operator-(const UInt8& a, const UInt8& b) { return UInt8(_mm_sub_epi32(a.low, b.low), _mm_sub_epi32(a.high, b.high)); }
__forceinline UInt8 operator-(const UInt8& a, int b) { return a - UInt8(b); }
__forceinline UInt8 operator-(int a, const UInt8& b) { return UInt8(a) - b; }

__forceinline UInt8 operator&(const UInt8& a, const UInt8& b) { return _mm256_castps_si256(_mm256_and_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
__forceinline UInt8 operator&(const UInt8& a, int b) { return a & UInt8(b); }
__forceinline UInt8 operator&(int a, const UInt8& b) { return UInt8(a) & b; }

__forceinline UInt8 operator|(const UInt8& a, const UInt8& b) { return _mm256_castps_si256(_mm256_or_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
__forceinline UInt8 operator|(const UInt8& a, int b) { return a | UInt8(b); }
__forceinline UInt8 operator|(int a, const UInt8& b) { return UInt8(a) | b; }

__forceinline UInt8 operator^(const UInt8& a, const UInt8& b) { return _mm256_castps_si256(_mm256_xor_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
__forceinline UInt8 operator^(const UInt8& a, int b) { return a ^ UInt8(b); }
__forceinline UInt8 operator^(int a, const UInt8& b) { return UInt8(a) ^ b; }

__forceinline UInt8 operator<<(const UInt8& a, int n) { return UInt8(_mm_slli_epi32(a.low, n), _mm_slli_epi32(a.high, n)); }
__forceinline UInt8 operator>>(const UInt8& a, int n) { return UInt8(_mm_srai_epi32(a.low, n), _mm_srai_epi32(a.high, n)); }


// Assignment operators
__forceinline UInt8& operator+=(UInt8& a, const UInt8& b) { return a = a + b; }
__forceinline UInt8& operator+=(UInt8& a, int b) { return a = a + b; }

__forceinline UInt8& operator-=(UInt8& a, const UInt8& b) { return a = a - b; }
__forceinline UInt8& operator-=(UInt8& a, int b) { return a = a - b; }

__forceinline UInt8& operator&=(UInt8& a, const UInt8& b) { return a = a & b; }
__forceinline UInt8& operator&=(UInt8& a, int b) { return a = a & b; }

__forceinline UInt8& operator|=(UInt8& a, const UInt8& b) { return a = a | b; }
__forceinline UInt8& operator|=(UInt8& a, int b) { return a = a | b; }

__forceinline UInt8& operator^=(UInt8& a, const UInt8& b) { return a = a ^ b; }
__forceinline UInt8& operator^=(UInt8& a, int b) { return a = a ^ b; }

__forceinline UInt8& operator<<=(UInt8& a, int n) { return a = a << n; }
__forceinline UInt8& operator>>=(UInt8& a, int n) { return a = a >> n; }


// Comparison operators
__forceinline Bool8 operator==(const UInt8& a, const UInt8& b)
{
	return Bool8(_mm_castsi128_ps(_mm_cmpeq_epi32(a.low, b.low)), _mm_castsi128_ps(_mm_cmpeq_epi32(a.high, b.high)));
}
__forceinline Bool8 operator!=(const UInt8& a, const UInt8& b) { return !(a == b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline UInt8 Shuffle(const UInt8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i0, int i1, int i2, int i3>
__forceinline UInt8 Shuffle(const UInt8& a, const UInt8& b)
{
	return _mm256_permute2f128_si256(a, b, (i1 << 4) | (i0 << 0));
}

template <int i>
__forceinline UInt8 Shuffle(const UInt8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i, i, i, i)));
}

// Misc math methods
__forceinline UInt8 Select(const Bool8& m, const UInt8& t, const UInt8& f)
{
	return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(f), _mm256_castsi256_ps(t), m));
}

#endif