//
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//

#pragma once

template <>
struct Float<4>
{
	// Constructors
	__forceinline Float() = default;
	__forceinline Float(const Float& other) : v(other.v) {}
	__forceinline Float(__m128 v) : v(v) {}
	__forceinline Float(float a) : v(_mm_set1_ps(a)) {}
	__forceinline Float(float a, float b, float c, float d) : v(_mm_set_ps(d, c, b, a)) {}
	__forceinline explicit Float(const Int4& a) : v(_mm_cvtepi32_ps(a)) {}


	// Assignment operator
	__forceinline Float& operator=(const Float4& other)
	{
		v = other.v;
		return *this;
	}


	// Type conversion operators
	__forceinline operator const __m128&() const { return v; }
	__forceinline operator __m128&() { return v; }


	// Load/store methods
	static __forceinline Float Load(const void* ptr)
	{
		return _mm_load_ps((float*)ptr);
	}

	static __forceinline Float LoadU(const void* ptr)
	{
		return _mm_loadu_ps((float*)ptr);
	}

	static __forceinline void Store(void* ptr, const Float& a)
	{
		_mm_store_ps((float*)ptr, a);
	}

	static __forceinline void StoreU(void* ptr, const Float& a)
	{
		_mm_storeu_ps((float*)ptr, a);
	}


	// Broadcast methods
	static __forceinline Float Broadcast(float a)
	{
		return _mm_broadcastss_ps(_mm_set1_ps(a));
	}


	// Array access
	__forceinline const float& operator[](size_t index) const
	{
		assert(index < 4);
		return f[index];
	}
	
	__forceinline float& operator[](size_t index)
	{
		assert(index < 4);
		return f[index];
	}


	static const size_t size = 4;
	union 
	{
		__m128 v;
		float f[4];
		int i[4];
	};
};


// Unary operators
__forceinline Float4 operator+(const Float4& a) { return a; }
__forceinline Float4 operator-(const Float4& a) { return _mm_xor_ps(a, simd_cast<__m128>(_mm_set1_epi32(0x80000000))); }


// Binary operators
__forceinline Float4 operator+(const Float4& a, const Float4& b) { return _mm_add_ps(a, b); }
__forceinline Float4 operator+(const Float4& a, float b) { return a + Float4(b); }
__forceinline Float4 operator+(float a, const Float4& b) { return Float4(a) + b; }

__forceinline Float4 operator-(const Float4& a, const Float4& b) { return _mm_sub_ps(a, b); }
__forceinline Float4 operator-(const Float4& a, float b) { return a - Float4(b); }
__forceinline Float4 operator-(float a, const Float4& b) { return Float4(a) - b; }

__forceinline Float4 operator*(const Float4& a, const Float4& b) { return _mm_mul_ps(a, b); }
__forceinline Float4 operator*(const Float4& a, float b) { return a * Float4(b); }
__forceinline Float4 operator*(float a, const Float4& b) { return Float4(a) * b; }

__forceinline Float4 operator/(const Float4& a, const Float4& b) { return _mm_div_ps(a, b); }
__forceinline Float4 operator/(const Float4& a, float b) { return a / Float4(b); }
__forceinline Float4 operator/(float a, const Float4& b) { return Float4(a) / b; }

__forceinline Float4 operator^(const Float4& a, const Float4& b) { return _mm_xor_ps(a, b); }
__forceinline Float4 operator^(const Float4& a, const Int4& b) { return _mm_xor_ps(a, simd_cast<__m128>(b)); }


// Assignment operators
__forceinline Float4& operator+=(Float4& a, const Float4& b) { return a = a + b; }
__forceinline Float4& operator+=(Float4& a, float b) { return a = a + b; }
__forceinline Float4& operator-=(Float4& a, const Float4& b) { return a = a - b; }
__forceinline Float4& operator-=(Float4& a, float b) { return a = a - b; }
__forceinline Float4& operator*=(Float4& a, const Float4& b) { return a = a * b; }
__forceinline Float4& operator*=(Float4& a, float b) { return a = a * b; }
__forceinline Float4& operator/=(Float4& a, const Float4& b) { return a = a / b; }
__forceinline Float4& operator/=(Float4& a, float b) { return a = a / b; }


// Comparison operators
__forceinline Bool4 operator==(const Float4& a, const Float4& b) { return _mm_cmpeq_ps(a, b); }
__forceinline Bool4 operator<(const Float4& a, const Float4& b) { return _mm_cmplt_ps(a, b); }
__forceinline Bool4 operator>(const Float4& a, const Float4& b) { return _mm_cmpgt_ps(a, b); }
__forceinline Bool4 operator!=(const Float4& a, const Float4& b) { return _mm_cmpneq_ps(a, b); }
__forceinline Bool4 operator<=(const Float4& a, const Float4& b) { return _mm_cmpngt_ps(a, b); }
__forceinline Bool4 operator>=(const Float4& a, const Float4& b) { return _mm_cmpnlt_ps(a, b); }

__forceinline Bool4 operator==(const Float4& a, float b) { return a == Float4(b); }
__forceinline Bool4 operator==(float a, const Float4& b) { return Float4(a) == b; }
__forceinline Bool4 operator<(const Float4& a, float b) { return a < Float4(b); }
__forceinline Bool4 operator<(float a, const Float4& b) { return Float4(a) < b; }
__forceinline Bool4 operator>(const Float4& a, float b) { return a > Float4(b); }
__forceinline Bool4 operator>(float a, const Float4& b) { return Float4(a) > b; }
__forceinline Bool4 operator!=(const Float4& a, float b) { return a != Float4(b); }
__forceinline Bool4 operator!=(float a, const Float4& b) { return Float4(a) != b; }
__forceinline Bool4 operator<=(const Float4& a, float b) { return a <= Float4(b); }
__forceinline Bool4 operator<=(float a, const Float4& b) { return Float4(a) <= b; }
__forceinline Bool4 operator>=(const Float4& a, float b) { return a >= Float4(b); }
__forceinline Bool4 operator>=(float a, const Float4& b) { return Float4(a) >= b; }


// Min/max functions
__forceinline Float4 Min(const Float4& a, const Float4& b) { return _mm_min_ps(a, b); }
__forceinline Float4 Max(const Float4& a, const Float4& b) { return _mm_max_ps(a, b); }

__forceinline Float4 Min(const Float4& a, float b) { return Min(a, Float4(b)); }
__forceinline Float4 Min(float a, const Float4& b) { return Min(Float4(a), b); }

__forceinline Float4 Max(const Float4& a, float b) { return Max(a, Float4(b)); }
__forceinline Float4 Max(float a, const Float4& b) { return Max(Float4(a), b); }


// Shuffle methods
template <int i0, int i1, int i2, int i3>
__forceinline Float4 Shuffle(const Float4& a)
{
	return simd_cast<__m128>(_mm_shuffle_epi32(simd_cast<__m128i>(a), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template <int i0, int i1, int i2, int i3>
__forceinline Float4 Shuffle(const Float4& a, const Float4& b)
{
	return _mm_shuffle_ps(a, b, _MM_SHUFFLE(i3, i2, i1, i0));
}

template <int i>
__forceinline Float4 Shuffle4(const Float4& a)
{
	return Shuffle<i, i, i, i>(a);
}

template <int i0, int i1, int i2, int i3>
__forceinline Float4 Shuffle(float a, float b)
{
	return Shuffle<i0, i1, i2, i3>(Float4(a), Float4(b));
}


// Reductions
__forceinline Float4 ReduceMin4(const Float4& a)
{
	Float4 b = Min(Shuffle<1, 0, 3, 2>(a), a);
	return Min(Shuffle<2, 3, 0, 1>(b), b);
}

__forceinline Float4 ReduceMax4(const Float4& a)
{
	Float4 b = Max(Shuffle<1, 0, 3, 2>(a), a);
	return Max(Shuffle<2, 3, 0, 1>(b), b);
}

__forceinline Float4 ReduceAdd4(const Float4& a)
{
	Float4 b = Shuffle<1, 0, 3, 2>(a) + a;
	return Shuffle<2, 3, 0, 1>(b) + b;
}

__forceinline float ReduceMin(const Float4& a)
{
	return _mm_cvtss_f32(ReduceMin4(a));
}

__forceinline float ReduceMax(const Float4& a)
{
	return _mm_cvtss_f32(ReduceMax4(a));
}

__forceinline float ReduceAdd(const Float4& a)
{
	return _mm_cvtss_f32(ReduceAdd4(a));
}


// Misc math methods
__forceinline Float4 Sqrt(const Float4& a) { return _mm_sqrt_ps(a); }
__forceinline Float4 Select(const Bool4& m, const Float4& t, const Float4& f)
{
	return _mm_blendv_ps(f, t, m);
}