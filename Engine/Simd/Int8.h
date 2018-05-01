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
struct Int<8>
{
	// Constructors
	__forceinline Int() = default;
	__forceinline Int(const Int& other) : v(other.v) {}
	__forceinline Int(__m256i v) : v(v) {}
	__forceinline explicit Int(__m256 v) : v(_mm256_cvtps_epi32(v)) {}
	__forceinline Int(int a) : v(_mm256_set1_epi32(a)) {}
	__forceinline Int(int a, int b) : v(_mm256_set_epi32(b, a, b, a, b, a, b, a)) {}
	__forceinline Int(int a, int b, int c, int d) 
		: v(_mm256_set_epi32(d, c, b, a, d, c, b, a)) 
	{}
	__forceinline Int(int a, int b, int c, int d, int e, int f, int g, int h) 
		: v(_mm256_set_epi32(h, g, f, e, d, c, b, a)) 
	{}


	// Assignment operator
	__forceinline Int& operator=(const Int& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m256i&() const { return v; }
	__forceinline operator __m256i&() { return v; }


	// Load/store methods
	static __forceinline Int Load(const void* ptr)
	{
		return _mm256_load_si256((__m256i*)ptr);
	}

	static __forceinline Int LoadU(const void* ptr)
	{
		return _mm256_loadu_si256((__m256i*)ptr);
	}

	static __forceinline void Store(void* ptr, const Int& a)
	{
		_mm256_store_si256((__m256i*)ptr, a);
	}

	static __forceinline void StoreU(void* ptr, const Int& a)
	{
		_mm256_storeu_si256((__m256i*)ptr, a);
	}


	// Array access
	__forceinline const int& operator[](size_t index) const
	{
		assert(index < 8);
		return i[index];
	}
	__forceinline int& operator[](size_t index)
	{
		assert(index < 8);
		return i[index];
	}


	static const size_t size = 8;
	union
	{
		__m256i v;
		int i[8];
	};
};


// Unary operators
__forceinline Int8 operator+(const Int8& a) { return a; }
__forceinline Int8 operator-(const Int8& a) { return _mm256_sub_epi32(_mm256_setzero_si256(), a); }


// Binary operators
__forceinline Int8 operator+(const Int8& a, const Int8& b) { return _mm256_add_epi32(a, b); }
__forceinline Int8 operator+(const Int8& a, int b) { return a + Int8(b); }
__forceinline Int8 operator+(int a, const Int8& b) { return Int8(a) + b; }

__forceinline Int8 operator-(const Int8& a, const Int8& b) { return _mm256_sub_epi32(a, b); }
__forceinline Int8 operator-(const Int8& a, int b) { return a - Int8(b); }
__forceinline Int8 operator-(int a, const Int8& b) { return Int8(a) - b; }

__forceinline Int8 operator*(const Int8& a, const Int8& b) { return _mm256_mullo_epi32(a, b); }
__forceinline Int8 operator*(const Int8& a, int b) { return a * Int8(b); }
__forceinline Int8 operator*(int a, const Int8& b) { return Int8(a) * b; }

__forceinline Int8 operator&(const Int8& a, const Int8& b) { return _mm256_and_si256(a, b); }
__forceinline Int8 operator&(const Int8& a, int b) { return a & Int8(b); }
__forceinline Int8 operator&(int a, const Int8& b) { return Int8(a) & b; }

__forceinline Int8 operator|(const Int8& a, const Int8& b) { return _mm256_or_si256(a, b); }
__forceinline Int8 operator|(const Int8& a, int b) { return a | Int8(b); }
__forceinline Int8 operator|(int a, const Int8& b) { return Int8(a) | b; }

__forceinline Int8 operator^(const Int8& a, const Int8& b) { return _mm256_xor_si256(a, b); }
__forceinline Int8 operator^(const Int8& a, int b) { return a ^ Int8(b); }
__forceinline Int8 operator^(int a, const Int8& b) { return Int8(a) ^ b; }

__forceinline Int8 operator<<(const Int8& a, int n) { return _mm256_slli_epi32(a, n); }
__forceinline Int8 operator>>(const Int8& a, int n) { return _mm256_srai_epi32(a, n); }


// Assignment operators
__forceinline Int8& operator+=(Int8& a, const Int8& b) { return a = a + b; }
__forceinline Int8& operator+=(Int8& a, int b) { return a = a + b; }

__forceinline Int8& operator-=(Int8& a, const Int8& b) { return a = a - b; }
__forceinline Int8& operator-=(Int8& a, int b) { return a = a - b; }

__forceinline Int8 operator*=(Int8& a, const Int8& b) { return a = a * b; }
__forceinline Int8 operator*=(Int8& a, int b) { return a = a * b; }

__forceinline Int8& operator&=(Int8& a, const Int8& b) { return a = a & b; }
__forceinline Int8& operator&=(Int8& a, int b) { return a = a & b; }

__forceinline Int8& operator|=(Int8& a, const Int8& b) { return a = a | b; }
__forceinline Int8& operator|=(Int8& a, int b) { return a = a | b; }

__forceinline Int8& operator^=(Int8& a, const Int8& b) { return a = a ^ b; }
__forceinline Int8& operator^=(Int8& a, int b) { return a = a ^ b; }

__forceinline Int8& operator<<=(Int8& a, int n) { return a = a << n; }
__forceinline Int8& operator>>=(Int8& a, int n) { return a = a >> n; }


// Comparison operators
__forceinline Bool8 operator==(const Int8& a, const Int8& b) { return _mm256_castsi256_ps(_mm256_cmpeq_epi32(a, b)); }
__forceinline Bool8 operator<(const Int8& a, const Int8& b) { return _mm256_castsi256_ps(_mm256_cmpgt_epi32(b, a)); }
__forceinline Bool8 operator>(const Int8& a, const Int8& b) { return _mm256_castsi256_ps(_mm256_cmpgt_epi32(a, b)); }
__forceinline Bool8 operator!=(const Int8& a, const Int8& b) { return !(a == b); }
__forceinline Bool8 operator<=(const Int8& a, const Int8& b) { return !(a > b); }
__forceinline Bool8 operator>=(const Int8& a, const Int8& b) { return !(a < b); }

__forceinline Bool8 operator==(const Int8& a, int b) { return a == Int8(b); }
__forceinline Bool8 operator==(int a, const Int8& b) { return Int8(a) == b; }
__forceinline Bool8 operator<(const Int8& a, int b) { return a < Int8(b); }
__forceinline Bool8 operator<(int a, const Int8& b) { return Int8(a) < b; }
__forceinline Bool8 operator>(const Int8& a, int b) { return a > Int8(b); }
__forceinline Bool8 operator>(int a, const Int8& b) { return Int8(a) > b; }
__forceinline Bool8 operator!=(const Int8& a, int b) { return a != Int8(b); }
__forceinline Bool8 operator!=(int a, const Int8& b) { return Int8(a) != b; }
__forceinline Bool8 operator<=(const Int8& a, int b) { return a <= Int8(b); }
__forceinline Bool8 operator<=(int a, const Int8& b) { return Int8(a) <= b; }
__forceinline Bool8 operator>=(const Int8& a, int b) { return a >= Int8(b); }
__forceinline Bool8 operator>=(int a, const Int8& b) { return Int8(a) >= b; }


// Min/max functions
__forceinline Int8 Min(const Int8& a, const Int8& b) { return _mm256_min_epi32(a, b); }
__forceinline Int8 Max(const Int8& a, const Int8& b) { return _mm256_max_epi32(a, b); }

__forceinline Int8 Min(const Int8& a, int b) { return Min(a, Int8(b)); }
__forceinline Int8 Min(int a, const Int8& b) { return Min(Int8(a), b); }

__forceinline Int8 Max(const Int8& a, int b) { return Max(a, Int8(b)); }
__forceinline Int8 Max(int a, const Int8& b) { return Max(Int8(a), b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline Int8 Shuffle(const Int8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i0, int i1, int i2, int i3>
__forceinline Int8 Shuffle(const Int8& a, const Int8& b)
{
	return _mm256_permute2f128_si256(a, b, (i1 << 4) | (i0 << 0));
}

template <int i>
__forceinline Int8 Shuffle(const Int8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i, i, i, i)));
}

#else

template <>
struct Int<8>
{
	// Constructors
	__forceinline Int() = default;
	__forceinline Int(const Int& other) : v(other.v) {}
	__forceinline Int(__m256i v) : v(v) {}
	__forceinline Int(const __m128i& a, const __m128i& b) : low(a), high(b) {}
	__forceinline explicit Int(__m256 v) : v(_mm256_cvtps_epi32(v)) {}
	__forceinline Int(int a) : v(_mm256_set1_epi32(a)) {}
	__forceinline Int(int a, int b) : v(_mm256_set_epi32(b, a, b, a, b, a, b, a)) {}
	__forceinline Int(int a, int b, int c, int d) 
		: v(_mm256_set_epi32(d, c, b, a, d, c, b, a)) 
	{}
	__forceinline Int(int a, int b, int c, int d, int e, int f, int g, int h) 
		: v(_mm256_set_epi32(h, g, f, e, d, c, b, a)) 
	{}


	// Assignment operator
	__forceinline Int8& operator=(const Int8& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m256i&() const { return v; }
	__forceinline operator __m256i&() { return v; }


	// Load/store methods
	static __forceinline Int Load(const void* ptr)
	{
		return _mm256_castps_si256(_mm256_load_ps((float*)ptr));
	}

	static __forceinline Int LoadU(const void* ptr)
	{
		return  _mm256_castps_si256(_mm256_loadu_ps((float*)ptr));
	}

	static __forceinline void Store(void* ptr, const Int& a)
	{
		_mm256_store_ps((float*)ptr, _mm256_castsi256_ps(a));
	}

	static __forceinline void StoreU(void* ptr, const Int& a)
	{
		_mm256_storeu_ps((float*)ptr, _mm256_castsi256_ps(a));
	}


	// Array access
	__forceinline const int& operator[](size_t index) const
	{
		assert(index < 8);
		return i[index];
	}
	__forceinline int& operator[](size_t index)
	{
		assert(index < 8);
		return i[index];
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
		int i[8];
	};
};


// Unary operators
__forceinline Int8 operator+(const Int8& a) { return a; }
__forceinline Int8 operator-(const Int8& a) { Int8(_mm_sub_epi32(_mm_setzero_si128(), a.low), _mm_sub_epi32(_mm_setzero_si128(), a.high)); }


// Binary operators
__forceinline Int8 operator+(const Int8& a, const Int8& b) { return Int8(_mm_add_epi32(a.low, b.low), _mm_add_epi32(a.high, b.high)); }
__forceinline Int8 operator+(const Int8& a, int b) { return a + Int8(b); }
__forceinline Int8 operator+(int a, const Int8& b) { return Int8(a) + b; }

__forceinline Int8 operator-(const Int8& a, const Int8& b) { return Int8(_mm_sub_epi32(a.low, b.low), _mm_sub_epi32(a.high, b.high)); }
__forceinline Int8 operator-(const Int8& a, int b) { return a - Int8(b); }
__forceinline Int8 operator-(int a, const Int8& b) { return Int8(a) - b; }

__forceinline Int8 operator*(const Int8& a, const Int8& b) { return Int8(_mm_mullo_epi32(a.low, b.low), _mm_mullo_epi32(a.high, b.high)); }
__forceinline Int8 operator*(const Int8& a, int b) { return a * Int8(b); }
__forceinline Int8 operator*(int a, const Int8& b) { return Int8(a) * b; }

__forceinline Int8 operator&(const Int8& a, const Int8& b) { return _mm256_castps_si256(_mm256_and_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
__forceinline Int8 operator&(const Int8& a, int b) { return a & Int8(b); }
__forceinline Int8 operator&(int a, const Int8& b) { return Int8(a) & b; }

__forceinline Int8 operator|(const Int8& a, const Int8& b) { return _mm256_castps_si256(_mm256_or_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
__forceinline Int8 operator|(const Int8& a, int b) { return a | Int8(b); }
__forceinline Int8 operator|(int a, const Int8& b) { return Int8(a) | b; }

__forceinline Int8 operator^(const Int8& a, const Int8& b) { return _mm256_castps_si256(_mm256_xor_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
__forceinline Int8 operator^(const Int8& a, int b) { return a ^ Int8(b); }
__forceinline Int8 operator^(int a, const Int8& b) { return Int8(a) ^ b; }

__forceinline Int8 operator<<(const Int8& a, int n) { return Int8(_mm_slli_epi32(a.low, n), _mm_slli_epi32(a.high, n)); }
__forceinline Int8 operator>>(const Int8& a, int n) { return Int8(_mm_srai_epi32(a.low, n), _mm_srai_epi32(a.high, n)); }


// Assignment operators
__forceinline Int8& operator+=(Int8& a, const Int8& b) { return a = a + b; }
__forceinline Int8& operator+=(Int8& a, int b) { return a = a + b; }

__forceinline Int8& operator-=(Int8& a, const Int8& b) { return a = a - b; }
__forceinline Int8& operator-=(Int8& a, int b) { return a = a - b; }

__forceinline Int8 operator*=(Int8& a, const Int8& b) { return a = a * b; }
__forceinline Int8 operator*=(Int8& a, int b) { return a = a * b; }

__forceinline Int8& operator&=(Int8& a, const Int8& b) { return a = a & b; }
__forceinline Int8& operator&=(Int8& a, int b) { return a = a & b; }

__forceinline Int8& operator|=(Int8& a, const Int8& b) { return a = a | b; }
__forceinline Int8& operator|=(Int8& a, int b) { return a = a | b; }

__forceinline Int8& operator^=(Int8& a, const Int8& b) { return a = a ^ b; }
__forceinline Int8& operator^=(Int8& a, int b) { return a = a ^ b; }

__forceinline Int8& operator<<=(Int8& a, int n) { return a = a << n; }
__forceinline Int8& operator>>=(Int8& a, int n) { return a = a >> n; }


// Comparison operators
__forceinline Bool8 operator==(const Int8& a, const Int8& b) 
{
	return Bool8(_mm_castsi128_ps(_mm_cmpeq_epi32(a.low, b.low)), _mm_castsi128_ps(_mm_cmpeq_epi32(a.high, b.high)));
}
__forceinline Bool8 operator<(const Int8& a, const Int8& b) 
{ 
	return Bool8(_mm_castsi128_ps(_mm_cmplt_epi32(a.low, b.low)), _mm_castsi128_ps(_mm_cmplt_epi32(a.high, b.high)));
}
__forceinline Bool8 operator>(const Int8& a, const Int8& b) 
{ 
	return Bool8(_mm_castsi128_ps(_mm_cmpgt_epi32(a.low, b.low)), _mm_castsi128_ps(_mm_cmpgt_epi32(a.high, b.high)));
}
__forceinline Bool8 operator!=(const Int8& a, const Int8& b) { return !(a == b); }
__forceinline Bool8 operator<=(const Int8& a, const Int8& b) { return !(a > b); }
__forceinline Bool8 operator>=(const Int8& a, const Int8& b) { return !(a < b); }

__forceinline Bool8 operator==(const Int8& a, int b) { return a == Int8(b); }
__forceinline Bool8 operator==(int a, const Int8& b) { return Int8(a) == b; }
__forceinline Bool8 operator<(const Int8& a, int b) { return a < Int8(b); }
__forceinline Bool8 operator<(int a, const Int8& b) { return Int8(a) < b; }
__forceinline Bool8 operator>(const Int8& a, int b) { return a > Int8(b); }
__forceinline Bool8 operator>(int a, const Int8& b) { return Int8(a) > b; }
__forceinline Bool8 operator!=(const Int8& a, int b) { return a != Int8(b); }
__forceinline Bool8 operator!=(int a, const Int8& b) { return Int8(a) != b; }
__forceinline Bool8 operator<=(const Int8& a, int b) { return a <= Int8(b); }
__forceinline Bool8 operator<=(int a, const Int8& b) { return Int8(a) <= b; }
__forceinline Bool8 operator>=(const Int8& a, int b) { return a >= Int8(b); }
__forceinline Bool8 operator>=(int a, const Int8& b) { return Int8(a) >= b; }


// Min/max functions
__forceinline Int8 Min(const Int8& a, const Int8& b) { return Int8(_mm_min_epi32(a.low, b.low), _mm_min_epi32(a.high, b.high)); }
__forceinline Int8 Max(const Int8& a, const Int8& b) { return Int8(_mm_max_epi32(a.low, b.low), _mm_max_epi32(a.high, b.high)); }

__forceinline Int8 Min(const Int8& a, int b) { return Min(a, Int8(b)); }
__forceinline Int8 Min(int a, const Int8& b) { return Min(Int8(a), b); }

__forceinline Int8 Max(const Int8& a, int b) { return Max(a, Int8(b)); }
__forceinline Int8 Max(int a, const Int8& b) { return Max(Int8(a), b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline Int8 Shuffle(const Int8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i0, int i1, int i2, int i3>
__forceinline Int8 Shuffle(const Int8& a, const Int8& b)
{
	return _mm256_permute2f128_si256(a, b, (i1 << 4) | (i0 << 0));
}

template <int i>
__forceinline Int8 Shuffle(const Int8& a)
{
	return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i, i, i, i)));
}

#endif