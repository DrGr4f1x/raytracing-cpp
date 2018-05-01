//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

struct Float8
{
	// Constructors
	__forceinline Float8() = default;
	__forceinline Float8(const Float8& other) : v(other.v) {}
	__forceinline Float8(__m256 v) : v(v) {}
	__forceinline Float8(float a) : v(_mm256_set1_ps(a)) {}
	__forceinline Float8(float a, float b) : v(_mm256_set_ps(b, a, b, a, b, a, b, a)) {}
	__forceinline Float8(float a, float b, float c, float d) 
		: v(_mm256_set_ps(d, c, b, a, d, c, b, a)) 
	{}
	__forceinline Float8(float a, float b, float c, float d, float e, float f, float g, float h) 
		: v(_mm256_set_ps(h, g, f, e, d, c, b, a)) 
	{}
	__forceinline explicit Float8(const Int8& a) : v(_mm256_cvtepi32_ps(a)) {}


	// Assignment operator
	__forceinline Float8& operator=(const Float8& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m256&() const { return v; }
	__forceinline operator __m256&() { return v; }


	// Load/store methods
	static __forceinline Float8 Load(const void* ptr)
	{
		return _mm256_load_ps((float*)ptr);
	}

	static __forceinline Float8 LoadU(const void* ptr)
	{
		return _mm256_loadu_ps((float*)ptr);
	}

	static __forceinline void Store(void* ptr, const Float8& a)
	{
		_mm256_store_ps((float*)ptr, a);
	}

	static __forceinline void StoreU(void* ptr, const Float8& a)
	{
		_mm256_storeu_ps((float*)ptr, a);
	}


	// Broadcast methods
	static __forceinline Float8 Broadcast(float a)
	{
		return _mm256_broadcastss_ps(_mm_set1_ps(a));
	}


	// Array access
	__forceinline const float& operator[](size_t index) const
	{
		assert(index < 8);
		return f[index];
	}

	__forceinline float& operator[](size_t index)
	{
		assert(index < 8);
		return f[index];
	}


	static const size_t size = 8;
	union
	{
		__m256 v;
		float f[8];
		int i[8];
	};
};


// Unary operators
__forceinline Float8 operator+(const Float8& a) { return a; }
__forceinline Float8 operator-(const Float8& a) { return _mm256_xor_ps(a, _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000))); }


// Binary operators
__forceinline Float8 operator+(const Float8& a, const Float8& b) { return _mm256_add_ps(a, b); }
__forceinline Float8 operator+(const Float8& a, float b) { return a + Float8(b); }
__forceinline Float8 operator+(float a, const Float8& b) { return Float8(a) + b; }

__forceinline Float8 operator-(const Float8& a, const Float8& b) { return _mm256_sub_ps(a, b); }
__forceinline Float8 operator-(const Float8& a, float b) { return a - Float8(b); }
__forceinline Float8 operator-(float a, const Float8& b) { return Float8(a) - b; }

__forceinline Float8 operator*(const Float8& a, const Float8& b) { return _mm256_mul_ps(a, b); }
__forceinline Float8 operator*(const Float8& a, float b) { return a * Float8(b); }
__forceinline Float8 operator*(float a, const Float8& b) { return Float8(a) * b; }

__forceinline Float8 operator/(const Float8& a, const Float8& b) { return _mm256_div_ps(a, b); }
__forceinline Float8 operator/(const Float8& a, float b) { return a / Float8(b); }
__forceinline Float8 operator/(float a, const Float8& b) { return Float8(a) / b; }

__forceinline Float8 operator^(const Float8& a, const Float8& b) { return _mm256_xor_ps(a, b); }
__forceinline Float8 operator^(const Float8& a, const Int8& b) { return _mm256_xor_ps(a, _mm256_castsi256_ps(b)); }


// Assignment operators
__forceinline Float8& operator+=(Float8& a, const Float8& b) { return a = a + b; }
__forceinline Float8& operator+=(Float8& a, float b) { return a = a + b; }
__forceinline Float8& operator-=(Float8& a, const Float8& b) { return a = a - b; }
__forceinline Float8& operator-=(Float8& a, float b) { return a = a - b; }
__forceinline Float8& operator*=(Float8& a, const Float8& b) { return a = a * b; }
__forceinline Float8& operator*=(Float8& a, float b) { return a = a * b; }
__forceinline Float8& operator/=(Float8& a, const Float8& b) { return a = a / b; }
__forceinline Float8& operator/=(Float8& a, float b) { return a = a / b; }


// Comparison operators
__forceinline Bool8 operator==(const Float8& a, const Float8& b) { return _mm256_cmp_ps(a, b, _CMP_EQ_OQ); }
__forceinline Bool8 operator<(const Float8& a, const Float8& b) { return _mm256_cmp_ps(a, b, _CMP_LT_OQ); }
__forceinline Bool8 operator>(const Float8& a, const Float8& b) { return _mm256_cmp_ps(a, b, _CMP_GT_OQ); }
__forceinline Bool8 operator!=(const Float8& a, const Float8& b) { return _mm256_cmp_ps(a, b, _CMP_NEQ_OQ); }
__forceinline Bool8 operator<=(const Float8& a, const Float8& b) { return _mm256_cmp_ps(a, b, _CMP_LE_OQ); }
__forceinline Bool8 operator>=(const Float8& a, const Float8& b) { return _mm256_cmp_ps(a, b, _CMP_GT_OQ); }

__forceinline Bool8 operator==(const Float8& a, float b) { return a == Float8(b); }
__forceinline Bool8 operator==(float a, const Float8& b) { return Float8(a) == b; }
__forceinline Bool8 operator<(const Float8& a, float b) { return a < Float8(b); }
__forceinline Bool8 operator<(float a, const Float8& b) { return Float8(a) < b; }
__forceinline Bool8 operator>(const Float8& a, float b) { return a > Float8(b); }
__forceinline Bool8 operator>(float a, const Float8& b) { return Float8(a) > b; }
__forceinline Bool8 operator!=(const Float8& a, float b) { return a != Float8(b); }
__forceinline Bool8 operator!=(float a, const Float8& b) { return Float8(a) != b; }
__forceinline Bool8 operator<=(const Float8& a, float b) { return a <= Float8(b); }
__forceinline Bool8 operator<=(float a, const Float8& b) { return Float8(a) <= b; }
__forceinline Bool8 operator>=(const Float8& a, float b) { return a >= Float8(b); }
__forceinline Bool8 operator>=(float a, const Float8& b) { return Float8(a) >= b; }


// Min/max functions
__forceinline Float8 Min(const Float8& a, const Float8& b) { return _mm256_min_ps(a, b); }
__forceinline Float8 Max(const Float8& a, const Float8& b) { return _mm256_max_ps(a, b); }

__forceinline Float8 Min(const Float8& a, float b) { return Min(a, Float8(b)); }
__forceinline Float8 Min(float a, const Float8& b) { return Min(Float8(a), b); }

__forceinline Float8 Max(const Float8& a, float b) { return Max(a, Float8(b)); }
__forceinline Float8 Max(float a, const Float8& b) { return Max(Float8(a), b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline Float8 Shuffle(const Float8& a)
{
	return _mm256_permute_ps(a, _MM_SHUFFLE(i3, i2, i1, i0));
}

template <int i0, int i1, int i2, int i3>
__forceinline Float8 Shuffle(const Float8& a, const Float8& b)
{
	return _mm256_shuffle_ps(a, b, _MM_SHUFFLE(i3, i2, i1, i0));
}

template<int i0, int i1>
__forceinline Float8 Shuffle(const Float8& a) 
{
	return _mm256_permute2f128_ps(a, a, (i1 << 4) | (i0 << 0));
}

template<int i0, int i1>
__forceinline Float8 Shuffle(const Float8& a, const Float8& b) 
{
	return _mm256_permute2f128_ps(a, b, (i1 << 4) | (i0 << 0));
}

template <int i>
__forceinline Float8 Shuffle(const Float8& a)
{
	return _mm256_permute_ps(a, _MM_SHUFFLE(i, i, i, i));
}


// Reductions
__forceinline Float8 ReduceMin2(const Float8& a)
{
	return Min(a, Shuffle<1, 0, 3, 2>(a));
}

__forceinline Float8 ReduceMin4(const Float8& a)
{
	Float8 b = ReduceMin2(a);
	return Min(b, Shuffle<2, 3, 0, 1>(b));
}

__forceinline Float8 ReduceMin8(const Float8& a)
{
	Float8 b = ReduceMin4(a);
	return Min(b, Shuffle<1, 0>(b));
}

__forceinline Float8 ReduceMax2(const Float8& a)
{
	return Max(a, Shuffle<1, 0, 3, 2>(a));
}

__forceinline Float8 ReduceMax4(const Float8& a)
{
	Float8 b = ReduceMax2(a);
	return Max(b, Shuffle<2, 3, 0, 1>(b));
}

__forceinline Float8 ReduceMax8(const Float8& a)
{
	Float8 b = ReduceMax4(a);
	return Max(b, Shuffle<1, 0>(b));
}

__forceinline Float8 ReduceAdd2(const Float8& a)
{
	return a + Shuffle<1, 0, 3, 2>(a);
}

__forceinline Float8 ReduceAdd4(const Float8& a)
{
	Float8 b = ReduceAdd2(a);
	return b + Shuffle<2, 3, 0, 1>(b);
}

__forceinline Float8 ReduceAdd8(const Float8& a)
{
	Float8 b = ReduceAdd4(a);
	return b + Shuffle<1, 0>(b);
}

__forceinline float ReduceMin(const Float8& a)
{
	return _mm256_cvtss_f32(ReduceMin8(a));
}

__forceinline float ReduceMax(const Float8& a)
{
	return _mm256_cvtss_f32(ReduceMax8(a));
}

__forceinline float ReduceAdd(const Float8& a)
{
	return _mm256_cvtss_f32(ReduceAdd8(a));
}


// Misc math methods
__forceinline Float8 Sqrt(const Float8& a) { return _mm256_sqrt_ps(a); }
__forceinline Float8 Select(const Bool8& m, const Float8& t, const Float8& f)
{
	return _mm256_blendv_ps(f, t, m);
}