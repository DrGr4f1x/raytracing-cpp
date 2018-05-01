//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

template <>
struct Int<4>
{
	// Constructors
	__forceinline Int() = default;
	__forceinline Int(const Int& other) : v(other.v) {}
	__forceinline Int(__m128i v) : v(v) {}
	__forceinline explicit Int(__m128 v) : v(_mm_cvtps_epi32(v)) {}
	__forceinline Int(int a) : v(_mm_set1_epi32(a)) {}
	__forceinline Int(int a, int b, int c, int d) : v(_mm_set_epi32(d, c, b, a)) {}
	__forceinline Int(const Bool4& a) : v(_mm_castps_si128((__m128)a)) {}


	// Assignment operator
	__forceinline Int& operator=(const Int4& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m128i&() const {	return v; }
	__forceinline operator __m128i&() { return v; }


	// Load/store methods
	static __forceinline Int Load(const void* ptr)
	{
		return _mm_load_si128((__m128i*)ptr);
	}

	static __forceinline Int LoadU(const void* ptr)
	{
		return _mm_loadu_si128((__m128i*)ptr);
	}

	static __forceinline void Store(void* ptr, const Int& a)
	{
		_mm_store_si128((__m128i*)ptr, a);
	}

	static __forceinline void StoreU(void* ptr, const Int& a)
	{
		_mm_storeu_si128((__m128i*)ptr, a);
	}


	// Array access
	__forceinline const int& operator[](size_t index) const
	{
		assert(index < 4);
		return i[index];
	}
	__forceinline int& operator[](size_t index)
	{
		assert(index < 4);
		return i[index];
	}


	static const size_t size = 4;
	union 
	{
		__m128i v;
		int i[4];
	};
};


// Unary operators
__forceinline Int4 operator+(const Int4& a) { return a; }
__forceinline Int4 operator-(const Int4& a) { return _mm_sub_epi32(_mm_setzero_si128(), a); }


// Binary operators
__forceinline Int4 operator+(const Int4& a, const Int4& b) { return _mm_add_epi32(a, b); }
__forceinline Int4 operator+(const Int4& a, int b) { return a + Int4(b); }
__forceinline Int4 operator+(int a, const Int4& b) { return Int4(a) + b; }

__forceinline Int4 operator-(const Int4& a, const Int4& b) { return _mm_sub_epi32(a, b); }
__forceinline Int4 operator-(const Int4& a, int b) { return a - Int4(b); }
__forceinline Int4 operator-(int a, const Int4& b) { return Int4(a) - b; }

__forceinline Int4 operator*(const Int4& a, const Int4& b) { return _mm_mullo_epi32(a, b); }
__forceinline Int4 operator*(const Int4& a, int b) { return a * Int4(b); }
__forceinline Int4 operator*(int a, const Int4& b) { return Int4(a) * b; }

__forceinline Int4 operator&(const Int4& a, const Int4& b) { return _mm_and_si128(a, b); }
__forceinline Int4 operator&(const Int4& a, int b) { return a & Int4(b); }
__forceinline Int4 operator&(int a, const Int4& b) { return Int4(a) & b; }

__forceinline Int4 operator|(const Int4& a, const Int4& b) { return _mm_or_si128(a, b); }
__forceinline Int4 operator|(const Int4& a, int b) { return a | Int4(b); }
__forceinline Int4 operator|(int a, const Int4& b) { return Int4(a) | b; }

__forceinline Int4 operator^(const Int4& a, const Int4& b) { return _mm_xor_si128(a, b); }
__forceinline Int4 operator^(const Int4& a, int b) { return a ^ Int4(b); }
__forceinline Int4 operator^(int a, const Int4& b) { return Int4(a) ^ b; }

__forceinline Int4 operator<<(const Int4& a, int n) { return _mm_slli_epi32(a, n); }
__forceinline Int4 operator>>(const Int4& a, int n) { return _mm_srai_epi32(a, n); }


// Assignment operators
__forceinline Int4& operator+=(Int4& a, const Int4& b) { return a = a + b; }
__forceinline Int4& operator+=(Int4& a, int b) { return a = a + b; }

__forceinline Int4& operator-=(Int4& a, const Int4& b) { return a = a - b; }
__forceinline Int4& operator-=(Int4& a, int b) { return a = a - b; }

__forceinline Int4 operator*=(Int4& a, const Int4& b) { return a = a * b; }
__forceinline Int4 operator*=(Int4& a, int b) { return a = a * b; }

__forceinline Int4& operator&=(Int4& a, const Int4& b) { return a = a & b; }
__forceinline Int4& operator&=(Int4& a, int b) { return a = a & b; }

__forceinline Int4& operator|=(Int4& a, const Int4& b) { return a = a | b; }
__forceinline Int4& operator|=(Int4& a, int b) { return a = a | b; }

__forceinline Int4& operator^=(Int4& a, const Int4& b) { return a = a ^ b; }
__forceinline Int4& operator^=(Int4& a, int b) { return a = a ^ b; }

__forceinline Int4& operator<<=(Int4& a, int n) { return a = a << n; }
__forceinline Int4& operator>>=(Int4& a, int n) { return a = a >> n; }


// Comparison operators
__forceinline Bool4 operator==(const Int4& a, const Int4& b) { return _mm_castsi128_ps(_mm_cmpeq_epi32(a, b)); }
__forceinline Bool4 operator<(const Int4& a, const Int4& b) { return _mm_castsi128_ps(_mm_cmplt_epi32(a, b)); }
__forceinline Bool4 operator>(const Int4& a, const Int4& b) { return _mm_castsi128_ps(_mm_cmpgt_epi32(a, b)); }
__forceinline Bool4 operator!=(const Int4& a, const Int4& b) { return !(a == b); }
__forceinline Bool4 operator<=(const Int4& a, const Int4& b) { return !(a > b); }
__forceinline Bool4 operator>=(const Int4& a, const Int4& b) { return !(a < b); }

__forceinline Bool4 operator==(const Int4& a, int b) { return a == Int4(b); }
__forceinline Bool4 operator==(int a, const Int4& b) { return Int4(a) == b; }
__forceinline Bool4 operator<(const Int4& a, int b) { return a < Int4(b); }
__forceinline Bool4 operator<(int a, const Int4& b) { return Int4(a) < b; }
__forceinline Bool4 operator>(const Int4& a, int b) { return a > Int4(b); }
__forceinline Bool4 operator>(int a, const Int4& b) { return Int4(a) > b; }
__forceinline Bool4 operator!=(const Int4& a, int b) { return a != Int4(b); }
__forceinline Bool4 operator!=(int a, const Int4& b) { return Int4(a) != b; }
__forceinline Bool4 operator<=(const Int4& a, int b) { return a <= Int4(b); }
__forceinline Bool4 operator<=(int a, const Int4& b) { return Int4(a) <= b; }
__forceinline Bool4 operator>=(const Int4& a, int b) { return a >= Int4(b); }
__forceinline Bool4 operator>=(int a, const Int4& b) { return Int4(a) >= b; }


// Min/max functions
__forceinline Int4 Min(const Int4& a, const Int4& b) { return _mm_min_epi32(a, b); }
__forceinline Int4 Max(const Int4& a, const Int4& b) { return _mm_max_epi32(a, b); }

__forceinline Int4 Min(const Int4& a, int b) { return Min(a, Int4(b)); }
__forceinline Int4 Min(int a, const Int4& b) { return Min(Int4(a), b); }

__forceinline Int4 Max(const Int4& a, int b) { return Max(a, Int4(b)); }
__forceinline Int4 Max(int a, const Int4& b) { return Max(Int4(a), b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline Int4 Shuffle(const Int4& a) 
{
	return _mm_shuffle_epi32(a, _MM_SHUFFLE(i3, i2, i1, i0));
}

template <int i0, int i1, int i2, int i3>
__forceinline Int4 Shuffle(const Int4& a, const Int4& b) 
{
	return _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i>
__forceinline Int4 Shuffle(const Int4& a)
{
	return Shuffle<i, i, i, i>(a);
}