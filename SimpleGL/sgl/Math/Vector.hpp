/**
 * \author DikobrAz
 * File contains common vector operations. If SIMPLE_GL_USE_SSE is ON, then
 * 4-component float vectors will be optimized using sse by default.
 * Don't forget to use aligned containers for sse Vector4f defined in 'Containers.hpp'.
 * @see Matrix.hpp
 * @see Containers.hpp
 */

#ifndef SIMPLE_GL_MATH_VECTOR_HPP
#define SIMPLE_GL_MATH_VECTOR_HPP

#include "Base.hpp"
#include <cmath>
#include <iostream>
#include <cassert>
#include <limits>

#ifdef SIMPLE_GL_USE_SSE

#ifdef MSVC
#   include <mmintrin.h>
#else // __GNUC__
#   include <xmmintrin.h>
#   ifdef SIMPLE_GL_USE_SSE4
#       include <smmintrin.h>
#   elif defined(SIMPLE_GL_USE_SSE3)
#       include <pmmintrin.h>
#   else
#       include <emmintrin.h>
#   endif
#endif // __GNUC__

#endif // SIMPLE_GL_USE_SSE

#ifdef MSVC
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif

SGL_BEGIN_MATH_NAMESPACE

// Forward
template<typename T, int n, int m>
class Matrix;

/** Vector column. */
template<typename ValueType, int n>
class Matrix<ValueType, n, 1>
{
public:
    typedef Matrix<ValueType, n, 1> this_type;

    static const int num_rows     = n;
    static const int num_columns  = 1;
    static const int num_elements = n;

public:
    Matrix() {}
    Matrix(const this_type& vec) { std::copy(vec.arr, vec.arr + n, arr); }
    
	explicit Matrix(ValueType val)
    {
        std::fill(arr, arr + n, val);
    }

    template<typename T>
    explicit Matrix(const Matrix<T, n, 1>& vec)
    {
        std::copy(arr, arr + n, arr);
    }

    ValueType& operator [] (int i)
    {
        assert(i < n);
        return arr[i];
    }

    ValueType operator [] (int i) const
    {
        assert(i < n);
        return arr[i];
    }

public:
    ValueType arr[n];
};

/** Vector row. */
template<typename ValueType, int m>
class Matrix<ValueType, 1, m>
{
public:
    typedef Matrix<ValueType, 1, m> this_type;

    static const int num_rows     = 1;
    static const int num_columns  = m;
    static const int num_elements = m;

public:
    Matrix() {}
    Matrix(const this_type& vec) { std::copy(vec.arr, vec.arr + m, arr); }
  
    explicit Matrix(ValueType val)
    {
        std::fill(arr, arr + m, val);
    }

    template<typename T>
    explicit Matrix(const Matrix<T, 1, m>& vec)
    {
        std::copy(arr, arr + m, arr);
    }

    ValueType& operator [] (int i)
    {
        assert(i < m);
        return arr[i];
    }

    ValueType operator [] (int i) const
    {
        assert(i < m);
        return arr[i];
    }

public:
    ValueType arr[m];
};

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/** Vector column. */
template<>
class Matrix<float, 4, 1> :
    public sgl::Aligned<0x10>
{
public:
    typedef Matrix<float, 4, 1> this_type;

    static const int num_rows     = 4;
    static const int num_columns  = 1;
    static const int num_elements = 4;

public:
    Matrix() {}

    Matrix(__m128 sseReg) :
        m128(sseReg)
    {}

    Matrix(const this_type& vec) :
        m128(vec.m128)
    {
        m128 = _mm_set_ps(vec.w, vec.z, vec.y, vec.x);
    }

	explicit Matrix(float val)
	{
		m128 = _mm_set1_ps(val);
	}

    Matrix(float _x, float _y, float _z, float _w)
    {
        m128 = _mm_set_ps(_w, _z, _y, _x);
    }

    template<typename T>
    explicit Matrix(const Matrix<T, 4, 1>& vec) :
        x( static_cast<float>(vec.x) ),
        y( static_cast<float>(vec.y) ),
        z( static_cast<float>(vec.z) ),
        w( static_cast<float>(vec.w) )
    {}

    float& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    float operator [] (unsigned int i) const
    {
        assert(i < 4);
        return arr[i];
    }

    this_type& operator = (const this_type& rhs)
    {
        m128 = rhs.m128;
        return *this;
    }

public:
    union
    {
        float   arr[4];
        __m128  m128;
        struct {
            float   x,y,z,w;
        };
    };
};

/** Vector row. */
template<>
class Matrix<float, 1, 4> :
    public sgl::Aligned<0x10>
{
public:
    typedef MatrixBase<float, 1, 4>     base_type;
    typedef Matrix<float, 1, 4>         this_type;

    static const int num_rows     = 1;
    static const int num_columns  = 4;
    static const int num_elements = 4;

public:
    Matrix() {}

    Matrix(__m128 sseReg) :
        m128(sseReg)
    {}

    Matrix(const this_type& vec) :
        m128(vec.m128)
    {
        m128 = _mm_set_ps(vec.w, vec.z, vec.y, vec.x);
    }

	explicit Matrix(float val)
	{
		m128 = _mm_set1_ps(val);
	}

    Matrix(float _x, float _y, float _z, float _w)
    {
        m128 = _mm_set_ps(_w, _z, _y, _x);
    }

    template<typename T>
    explicit Matrix(const Matrix<T, 1, 4>& vec) :
        x( static_cast<float>(vec.x) ),
        y( static_cast<float>(vec.y) ),
        z( static_cast<float>(vec.z) ),
        w( static_cast<float>(vec.w) )
    {}

    float& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    float operator [] (unsigned int i) const
    {
        assert(i < 4);
        return arr[i];
    }

    this_type& operator = (const this_type& rhs)
    {
        m128 = rhs.m128;
        return *this;
    }

public:
    union
    {
        float       arr[4];
        __m128      m128;
        struct {
            float   x,y,z,w;
        };
    };
};

#endif // SIMPLE_GL_USE_SSE

/** Vector column. */
template<typename ValueType>
class Matrix<ValueType, 4, 1>
{
public:
    typedef Matrix<ValueType, 4, 1> this_type;

    static const int num_rows     = 4;
    static const int num_columns  = 1;
    static const int num_elements = 4;

public:
    Matrix() {}

    Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

	explicit Matrix(ValueType val) :
		x(val),
		y(val),
		z(val),
		w(val)
	{}

    Matrix(ValueType _x, ValueType _y, ValueType _z, ValueType _w) :
        x(_x),
        y(_y),
        z(_z),
        w(_w)
    {}

    template<typename T>
    explicit Matrix(const Matrix<T, 4, 1>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) ),
        w( static_cast<ValueType>(vec.w) )
    {}

    ValueType& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    ValueType operator [] (unsigned int i) const
    {
        assert(i < 4);
        return arr[i];
    }

public:
    union
    {
        ValueType       arr[4];
        struct {
            ValueType   x,y,z,w;
        };
    };
};

/** Vector row. */
template<typename ValueType>
class Matrix<ValueType, 1, 4>
{
public:
    typedef Matrix<ValueType, 1, 4> this_type;

    static const int num_rows     = 1;
    static const int num_columns  = 4;
    static const int num_elements = 4;

public:
    Matrix() {}

    Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

	explicit Matrix(ValueType val) :
		x(val),
		y(val),
		z(val),
		w(val)
	{}

    Matrix(ValueType _x, ValueType _y, ValueType _z, ValueType _w) :
        x(_x),
        y(_y),
        z(_z),
        w(_w)
    {}

    template<typename T>
    explicit Matrix(const Matrix<T, 1, 4>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) ),
        w( static_cast<ValueType>(vec.w) )
    {}

    ValueType& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    ValueType operator [] (unsigned int i) const
    {
        assert(i < 4);
        return arr[i];
    }

public:
    union
    {
        ValueType       arr[4];
        struct {
            ValueType   x,y,z,w;
        };
    };
};

/** Vector column. */
template<typename ValueType>
class Matrix<ValueType, 3, 1>
{
public:
    typedef Matrix<ValueType, 3, 1> this_type;

    static const int num_rows     = 3;
    static const int num_columns  = 1;
    static const int num_elements = 3;

public:
    Matrix() {}

    Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z)
    {}

	explicit Matrix(ValueType val) :
		x(val),
		y(val),
		z(val)
	{}

    Matrix(ValueType _x, ValueType _y, ValueType _z) :
        x(_x),
        y(_y),
        z(_z)
    {}

    template<typename T>
    explicit Matrix(const Matrix<T, 3, 1>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) )
    {}

    ValueType& operator [] (unsigned int i)
    {
        assert(i < 3);
        return arr[i];
    }

    ValueType operator [] (unsigned int i) const
    {
        assert(i < 3);
        return arr[i];
    }

public:
    union
    {
        ValueType       arr[3];
        struct {
            ValueType   x,y,z;
        };
    };
};

/** Vector row. */
template<typename ValueType>
class Matrix<ValueType, 1, 3>
{
public:
    typedef Matrix<ValueType, 1, 3> this_type;

    static const int num_rows     = 1;
    static const int num_columns  = 3;
    static const int num_elements = 3;

public:
    Matrix() {}

    Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z)
    {}

	explicit Matrix(ValueType val) :
		x(val),
		y(val),
		z(val)
	{}

    Matrix(ValueType _x, ValueType _y, ValueType _z) :
        x(_x),
        y(_y),
        z(_z)
    {}

    template<typename T>
    explicit Matrix(const Matrix<T, 1, 3>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) )
    {}

    ValueType& operator [] (unsigned int i)
    {
        assert(i < 3);
        return arr[i];
    }

    ValueType operator [] (unsigned int i) const
    {
        assert(i < 3);
        return arr[i];
    }

public:
    union
    {
        ValueType       arr[3];
        struct {
            ValueType   x,y,z;
        };
    };
};

/** Vector column. */
template<typename ValueType>
class Matrix<ValueType, 2, 1>
{
public:
    typedef Matrix<ValueType, 2, 1> this_type;

    static const int num_rows     = 2;
    static const int num_columns  = 1;
    static const int num_elements = 2;

public:
    Matrix() {}

    Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y)
    {}

	explicit Matrix(ValueType val) :
		x(val),
		y(val)
	{}

    Matrix(ValueType _x, ValueType _y) :
        x(_x),
        y(_y)
    {}

    template<typename T>
    explicit Matrix(const Matrix<T, 2, 1>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) )
    {}

    ValueType& operator [] (unsigned int i)
    {
        assert(i < 2);
        return arr[i];
    }

    ValueType operator [] (unsigned int i) const
    {
        assert(i < 2);
        return arr[i];
    }

public:
    union
    {
        ValueType       arr[2];
        struct {
            ValueType   x,y;
        };
    };
};

/** Vector row. */
template<typename ValueType>
class Matrix<ValueType, 1, 2>
{
public:
    typedef Matrix<ValueType, 1, 2> this_type;

    static const int num_rows     = 1;
    static const int num_columns  = 2;
    static const int num_elements = 2;

public:
    Matrix() {}

    Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y)
    {}

	explicit Matrix(ValueType val) :
		x(val),
		y(val)
	{}

    Matrix(ValueType _x, ValueType _y) :
        x(_x),
        y(_y)
    {}

    template<typename T>
    explicit Matrix(const Matrix<T, 1, 2>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) )
    {}

    ValueType& operator [] (unsigned int i)
    {
        assert(i < 2);
        return arr[i];
    }

    ValueType operator [] (unsigned int i) const
    {
        assert(i < 2);
        return arr[i];
    }

public:
    union
    {
        ValueType       arr[2];
        struct {
            ValueType   x,y;
        };
    };
};

// default vectors
typedef Matrix<double, 4, 1>           Vector4d;
typedef Matrix<double, 3, 1>           Vector3d;
typedef Matrix<double, 2, 1>           Vector2d;

typedef Matrix<float, 4, 1>            Vector4f;
typedef Matrix<float, 3, 1>            Vector3f;
typedef Matrix<float, 2, 1>            Vector2f;

typedef Matrix<int, 4, 1>              Vector4i;
typedef Matrix<int, 3, 1>              Vector3i;
typedef Matrix<int, 2, 1>              Vector2i;

typedef Matrix<unsigned int, 4, 1>     Vector4ui;
typedef Matrix<unsigned int, 3, 1>     Vector3ui;
typedef Matrix<unsigned int, 2, 1>     Vector2ui;

typedef Matrix<short, 4, 1>            Vector4s;
typedef Matrix<short, 3, 1>            Vector3s;
typedef Matrix<short, 2, 1>            Vector2s;

typedef Matrix<unsigned short, 4, 1>   Vector4us;
typedef Matrix<unsigned short, 3, 1>   Vector3us;
typedef Matrix<unsigned short, 2, 1>   Vector2us;

typedef Matrix<char, 4, 1>             Vector4c;
typedef Matrix<char, 3, 1>             Vector3c;
typedef Matrix<char, 2, 1>             Vector2c;

typedef Matrix<unsigned char, 4, 1>    Vector4uc;
typedef Matrix<unsigned char, 3, 1>    Vector3uc;
typedef Matrix<unsigned char, 2, 1>    Vector2uc;

typedef Matrix<bool, 4, 1>             Vector4b;
typedef Matrix<bool, 3, 1>             Vector3b;
typedef Matrix<bool, 2, 1>             Vector2b;

/** mul per component */
template<typename T, int n>
inline Matrix<T, n, 1>& operator *= (Matrix<T, n, 1>& lhs, const Matrix<T, n, 1>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs[i];
    return lhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, 1, n>& operator *= (Matrix<T, 1, n>& lhs, const Matrix<T, 1, n>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs[i];
    return lhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, n, 1>& operator *= (Matrix<T, n, 1>& lhs, T rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs;
    return lhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, 1, n>& operator *= (Matrix<T, 1, n>& lhs, T rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs;
    return lhs;
}

/** div per component */
template<typename T, int n>
inline Matrix<T, n, 1>& operator /= (Matrix<T, n, 1>& lhs, const Matrix<T, n, 1>& rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= rhs[i];
    return lhs;
}

/** div per component */
template<typename T, int n>
inline Matrix<T, 1, n>& operator /= (Matrix<T, 1, n>& lhs, const Matrix<T, 1, n>& rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= rhs[i];
    return lhs;
}

/** div per component */
template<typename T, typename Y, int n>
inline Matrix<T, n, 1>& operator /= (Matrix<T, n, 1>& lhs, Y rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= static_cast<T>(rhs);
    return lhs;
}

/** div per component */
template<typename T, typename Y, int n>
inline Matrix<T, 1, n>& operator /= (Matrix<T, 1, n>& lhs, Y rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= static_cast<T>(rhs);
    return lhs;
}

/** compare vectors */
template<typename T, int n>
inline bool operator == (const Matrix<T, n, 1>& lhs, const Matrix<T, n, 1>& rhs)
{
    for(int i = 0; i<n; ++i)
    {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

/** compare vectors */
template<typename T, int n>
inline bool operator == (const Matrix<T, 1, n>& lhs, const Matrix<T, 1, n>& rhs)
{
    for(int i = 0; i<n; ++i)
    {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

/** compare matrices */
template<typename T, int n>
inline bool operator != (const Matrix<T, n, 1>& lhs, const Matrix<T, n, 1>& rhs)
{
    for(int i = 0; i<n; ++i)
    {
        if (lhs[i] != rhs[i]) {
            return true;
        }
    }

    return false;
}

/** compare matrices */
template<typename T, int n>
inline bool operator != (const Matrix<T, 1, n>& lhs, const Matrix<T, 1, n>& rhs)
{
    for(int i = 0; i<n; ++i)
    {
        if (lhs[i] != rhs[i]) {
            return true;
        }
    }

    return false;
}

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/** add per component */
inline Matrix<float, 4, 1>& operator += (Matrix<float, 4, 1>& lhs, const Matrix<float, 4, 1>& rhs)
{
    lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** add per component */
inline Matrix<float, 1, 4>& operator += (Matrix<float, 1, 4>& lhs, const Matrix<float, 1, 4>& rhs)
{
    lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** subtract per component */
inline Matrix<float, 4, 1>& operator -= (Matrix<float, 4, 1>& lhs, const Matrix<float, 4, 1>& rhs)
{
    lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** subtract per component */
inline Matrix<float, 1, 4>& operator -= (Matrix<float, 1, 4>& lhs, const Matrix<float, 1, 4>& rhs)
{
    lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** mul per component */
inline Matrix<float, 4, 1>& operator *= (Matrix<float, 4, 1>& lhs, const Matrix<float, 4, 1>& rhs)
{
    lhs.m128 = _mm_mul_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** mul per component */
inline Matrix<float, 1, 4>& operator *= (Matrix<float, 1, 4>& lhs, const Matrix<float, 1, 4>& rhs)
{
    lhs.m128 = _mm_mul_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** mul per component */
inline Matrix<float, 4, 1>& operator *= (Matrix<float, 4, 1>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128 = _mm_mul_ps(lhs.m128, packed);
    return lhs;
}

/** mul per component */
inline Matrix<float, 1, 4>& operator *= (Matrix<float, 1, 4>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128 = _mm_mul_ps(lhs.m128, packed);
    return lhs;
}

/** div per component */
inline Matrix<float, 4, 1>& operator /= (Matrix<float, 4, 1>& lhs, const Matrix<float, 4, 1>& rhs)
{
    lhs.m128 = _mm_div_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** div per component */
inline Matrix<float, 1, 4>& operator /= (Matrix<float, 1, 4>& lhs, const Matrix<float, 1, 4>& rhs)
{
    lhs.m128 = _mm_div_ps(lhs.m128, rhs.m128);
    return lhs;
}

#endif // SIMPLE_GL_USE_SSE

/** mul per component */
template<typename T, int n>
inline Matrix<T, n, 1> operator * (const Matrix<T, n, 1>& lhs, const Matrix<T, n, 1>& rhs)
{
    Matrix<T, n, 1> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, 1, n> operator * (const Matrix<T, 1, n>& lhs, const Matrix<T, 1, n>& rhs)
{
    Matrix<T, 1, n> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, n, 1> operator * (const Matrix<T, n, 1>& lhs, T rhs)
{
    Matrix<T, n, 1> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, 1, n> operator * (const Matrix<T, 1, n>& lhs, T rhs)
{
    Matrix<T, 1, n> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, n, 1> operator * (T lhs, const Matrix<T, n, 1>& rhs)
{
    Matrix<T, n, 1> tmp(rhs);
    return tmp *= lhs;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, 1, n> operator * (T lhs, const Matrix<T, 1, n>& rhs)
{
    Matrix<T, 1, n> tmp(rhs);
    return tmp *= lhs;
}

/** mul matrix per vector column */
template<typename T, int n, int m>
Matrix<T, n, 1> operator * (const Matrix<T, n, m>& lhs, const Matrix<T, m, 1>& rhs)
{
    Matrix<T, n, 1> res;
    for(int i = 0; i<n; ++i)
    {
        res[i] = lhs[i][0] * rhs[0];
        for(int k = 1; k<m; ++k)
            res[i] += lhs[i][k] * rhs[k];
    }
    return res;
}

/** mul matrix44 per vector4 */
template<typename T>
Matrix<T, 4, 1> operator * (const Matrix<T, 4, 4>& lhs, const Matrix<T, 4, 1>& rhs)
{
    Matrix<T, 4, 1> res;
    res[0] = lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2] + lhs[0][3] * rhs[3];
    res[1] = lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2] + lhs[1][3] * rhs[3];
    res[2] = lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2] + lhs[2][3] * rhs[3];
    res[3] = lhs[3][0] * rhs[0] + lhs[3][1] * rhs[1] + lhs[3][2] * rhs[2] + lhs[3][3] * rhs[3];
    return res;
}

/** mul matrix44 per vector3 */
template<typename T>
Matrix<T, 3, 1> operator * (const Matrix<T, 4, 4>& lhs, const Matrix<T, 3, 1>& rhs)
{
    Matrix<T, 3, 1> res;
    res[0] = lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2];
    res[1] = lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2];
    res[2] = lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2];
    return res;
}

/** mul column per row */
template<typename T, int n, int l>
Matrix<T, n, l> operator * (const Matrix<T, n, 1>& lhs, const Matrix<T, 1, l>& rhs)
{
    Matrix<T, n, l> res;
    for(int i = 0; i<n; ++i)
    {
        for(int j = 0; j<l; ++j) {
            res[i][j] = lhs[i] * rhs[j];
        }
    }
    return res;
}

/** mul vector row per matrix */
template<typename T,int m, int l>
Matrix<T, 1, l> operator * (const Matrix<T, 1, m>& lhs, const Matrix<T, m, l>& rhs)
{
    Matrix<T, 1, l> res;
    for(int j = 0; j<l; ++j)
    {
        res[j] = lhs[0] * rhs[0][j];
        for(int k = 1; k<m; ++k)
            res[j] += lhs[k] * rhs[k][j];
    }
    return res;
}

/** div per component */
template<typename T, int n>
inline Matrix<T, n, 1> operator / (const Matrix<T, n, 1>& lhs, const Matrix<T, n, 1>& rhs)
{
    Matrix<T, n, 1> tmp(lhs);
    return tmp /= rhs;
}

/** div per component */
template<typename T, int n>
inline Matrix<T, 1, n> operator / (const Matrix<T, 1, n>& lhs, const Matrix<T, 1, n>& rhs)
{
    Matrix<T, 1, n> tmp(lhs);
    return tmp /= rhs;
}

/** div per component */
template<typename T, int n>
inline Matrix<T, n, 1> operator / (const Matrix<T, n, 1>& lhs, T rhs)
{
    Matrix<T, n, 1> tmp(lhs);
    return tmp /= rhs;
}

/** div per component */
template<typename T, int n>
inline Matrix<T, 1, n> operator / (const Matrix<T, 1, n>& lhs, T rhs)
{
    Matrix<T, 1, n> tmp(lhs);
    return tmp /= rhs;
}

/** print vector */
template<typename T, int n>
std::ostream& operator << (std::ostream& os, const Matrix<T, n, 1>& vec)
{
    for(int i = 0; i<n-1; ++i) {
        os << vec[i] << ' ';
    }

    return os << vec[n-1];
}

/** print vector */
template<typename T, int n>
std::ostream& operator << (std::ostream& os, const Matrix<T, 1, n>& vec)
{
    for(int i = 0; i<n-1; ++i) {
        os << vec[i] << ' ';
    }

    return os << vec[n-1];
}

/** read vector */
template<typename T, int n>
std::istream& operator >> (std::istream& istream, Matrix<T, n, 1>& vec)
{
    for(int i = 0; i<n; ++i) {
        istream >> vec[i];
    }

    return istream;
}

/** read vector */
template<typename T, int n>
std::istream& operator >> (std::istream& istream, Matrix<T, 1, n>& vec)
{
    for(int i = 0; i<n; ++i) {
        istream >> vec[i];
    }

    return istream;
}

/** append value to vector */
template<typename T, int n>
inline Matrix<T, n+1, 1> make_vec(const Matrix<T, n, 1>& vec, T val)
{
    Matrix<T, n+1, 1> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n] = val;
    return res;
}

/** append value to vector */
template<typename T, int n>
inline Matrix<T, 1, n+1> make_vec(const Matrix<T, 1, n>& vec, T val)
{
    Matrix<T, 1, n+1> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n] = val;
    return res;
}

/** append 2 values to vector */
template<typename T, int n>
inline Matrix<T, n+2, 1> make_vec(const Matrix<T, n, 1>& vec, T a, T b)
{
    Matrix<T, n+2, 1> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n]   = a;
    res[n+1] = b;
    return res;
}

/** append 2 values to vector */
template<typename T, int n>
inline Matrix<T, 1, n+2> make_vec(const Matrix<T, 1, n>& vec, T a, T b)
{
    Matrix<T, 1, n+2> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n]   = a;
    res[n+1] = b;
    return res;
}

/** concatenate vectors */
template<typename T, int n, int m>
inline Matrix<T, n+m, 1> make_vec(const Matrix<T, n, 1>& a, const Matrix<T, m, 1>& b)
{
    Matrix<T, n+m, 1> res;
    std::copy(a.arr, a.arr + n, res.arr);
    std::copy(b.arr, b.arr + m, res.arr + n);
    return res;
}

/** concatenate vectors */
template<typename T, int n, int m>
inline Matrix<T, 1, n+m> make_vec(const Matrix<T, 1, n>& a, const Matrix<T, 1, m>& b)
{
    Matrix<T, 1, n+m> res;
    std::copy(a.arr, a.arr + n, res.arr);
    std::copy(b.arr, b.arr + m, res.arr + n);
    return res;
}

/** compare vector components absolute differences with zero. */
template<typename T, int n>
inline bool fpt_small( const Matrix<T, n, 1>& a,
                       T                      eps = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_small(a[i], eps) ) {
            return false;
        }
    }

    return true;
}

/** compare vector components absolute differences with zero. */
template<typename T, int n>
inline bool fpt_small( const Matrix<T, 1, n>& a,
                       T                      eps = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_small(a[i], eps) ) {
            return false;
        }
    }

    return true;
}

/** compare vector components absolute differences. */
template<typename T, int n>
inline bool fpt_close_abs( const Matrix<T, n, 1>& a,
                           const Matrix<T, n, 1>& b,
                           T                      eps = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_close_abs(a[i], b[i], eps) ) {
            return false;
        }
    }

    return true;
}

/** compare vector components absolute differences. */
template<typename T, int n>
inline bool fpt_close_abs( const Matrix<T, 1, n>& a,
                           const Matrix<T, 1, n>& b,
                           T                      eps = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_close_abs(a[i], b[i], eps) ) {
            return false;
        }
    }

    return true;
}

/** compare vector components relative differences. */
template<typename T, int n>
inline bool fpt_close_rel( const Matrix<T, n, 1>& a,
                           const Matrix<T, n, 1>& b,
                           T                      eps = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_close_rel(a[i], b[i], eps) ) {
            return false;
        }
    }

    return true;
}

/** compare vector components relative differences. */
template<typename T, int n>
inline bool fpt_close_rel( const Matrix<T, 1, n>& a,
                           const Matrix<T, 1, n>& b,
                           T                      eps = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_close_rel(a[i], b[i], eps) ) {
            return false;
        }
    }

    return true;
}

/** compare vector components both absolute and relative differences. */
template<typename T, int n>
inline bool fpt_close( const Matrix<T, n, 1>& a,
                       const Matrix<T, n, 1>& b,
                       T                      epsAbs = std::numeric_limits<T>::epsilon(),
                       T                      epsRel = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_close(a[i], b[i], epsAbs, epsRel) ) {
            return false;
        }
    }

    return true;
}

/** compare vector components both absolute and relative differences. */
template<typename T, int n>
inline bool fpt_close( const Matrix<T, 1, n>& a,
                       const Matrix<T, 1, n>& b,
                       T                      epsAbs = std::numeric_limits<T>::epsilon(),
                       T                      epsRel = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !fpt_close(a[i], b[i], epsAbs, epsRel) ) {
            return false;
        }
    }

    return true;
}

/** per component max */
template<typename T, int n>
inline Matrix<T, n, 1> max(const Matrix<T, n, 1>& a, const Matrix<T, n, 1>& b)
{
    Matrix<T, n, 1> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::max(a[i], b[i]);
    }
    return result;
}

/** per component max */
template<typename T, int n>
inline Matrix<T, 1, n> max(const Matrix<T, 1, n>& a, const Matrix<T, 1, n>& b)
{
    Matrix<T, 1, n> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::max(a[i], b[i]);
    }
    return result;
}

/** per component min */
template<typename T, int n>
inline Matrix<T, n, 1> min(const Matrix<T, n, 1>& a, const Matrix<T, n, 1>& b)
{
    Matrix<T, n, 1> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min(a[i], b[i]);
    }
    return result;
}

/** per component min */
template<typename T, int n>
inline Matrix<T, 1, n> min(const Matrix<T, 1, n>& a, const Matrix<T, 1, n>& b)
{
    Matrix<T, 1, n> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min(a[i], b[i]);
    }
    return result;
}

/** per component min & max*/
template<typename T, int n>
inline void min_max( const Matrix<T, n, 1>& a,
                     const Matrix<T, n, 1>& b,
                     Matrix<T, n, 1>&       outMin,
                     Matrix<T, n, 1>&       outMax )
{
    for(int i = 0; i<n; ++i)
    {
        if ( a[i] > b[i] )
        {
            outMin[i] = b[i];
            outMax[i] = a[i];
        }
        else
        {
            outMin[i] = a[i];
            outMax[i] = b[i];
        }
    }
}

/** per component min & max*/
template<typename T, int n>
inline void min_max( const Matrix<T, 1, n>& a,
                     const Matrix<T, 1, n>& b,
                     Matrix<T, 1, n>&       outMin,
                     Matrix<T, 1, n>&       outMax )
{
    for(int i = 0; i<n; ++i)
    {
        if ( a[i] > b[i] )
        {
            outMin[i] = b[i];
            outMax[i] = a[i];
        }
        else
        {
            outMin[i] = a[i];
            outMax[i] = b[i];
        }
    }
}

/** per component clamp */
template<typename T, int n>
inline Matrix<T, n, 1> clamp(const Matrix<T, n, 1>& vec, T minVal, T maxVal)
{
    Matrix<T, n, 1> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min( std::max(vec[i], minVal), maxVal );
    }
    return result;
}

/** per component clamp */
template<typename T, int n>
inline Matrix<T, 1, n> clamp(const Matrix<T, 1, n>& vec, T minVal, T maxVal)
{
    Matrix<T, 1, n> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min( std::max(vec[i], minVal), maxVal );
    }
    return result;
}

/** linear interpolation between vectors */
template<typename T, int n>
inline Matrix<T, n, 1> lerp(const Matrix<T, n, 1>& a, const Matrix<T, n, 1>& b, T lerpVal)
{
    Matrix<T, n, 1> result;
    for(int i = 0; i<n; ++i) {
        result[i] = a[i] + lerpVal * (b[i] - a[i]);
    }
    return result;
}

/** linear interpolation between vectors */
template<typename T, int n>
inline Matrix<T, 1, n> lerp(const Matrix<T, 1, n>& a, const Matrix<T, 1, n>& b, T lerpVal)
{
    Matrix<T, 1, n> result;
    for(int i = 0; i<n; ++i) {
        result[i] = a[i] + lerpVal * (b[i] - a[i]);
    }
    return result;
}

/** Same as lisp cdr. Returns vector without last element. */
template<typename T, int n>
inline Matrix<T, n - 1, 1> cdr(const Matrix<T, n, 1>& vec)
{
    Matrix<T, n - 1, 1> res;
    std::copy(vec.arr, vec.arr + n - 1, res.arr);
    return res;
}

/** Same as lisp cdr. Returns vector without last element. */
template<typename T, int n>
inline Matrix<T, 1, n - 1> cdr(const Matrix<T, 1, n>& vec)
{
    Matrix<T, 1, n - 1> res;
    std::copy(vec.arr, vec.arr + n - 1, res.arr);
    return res;
}

template<typename T, int n>
inline Matrix<T, 3, 1> xyz(const Matrix<T, n, 1>& vec)
{
    return Matrix<T, 3, 1>(vec.x, vec.y, vec.z);
}

template<typename T, int n>
inline Matrix<T, 1, 3> xyz(const Matrix<T, 1, n>& vec)
{
    return Matrix<T, 1, 3>(vec.x, vec.y, vec.z);
}

template<typename T, int n>
inline Matrix<T, 2, 1> xy(const Matrix<T, n, 1>& vec)
{
    return Matrix<T, 2, 1>(vec.x, vec.y);
}

template<typename T, int n>
inline Matrix<T, 1, 2> xy(const Matrix<T, 1, n>& vec)
{
    return Matrix<T, 1, 2>(vec.x, vec.y);
}

/** dot product */
template<typename T, int n>
inline T dot( const Matrix<T, n, 1>& lhs,
              const Matrix<T, n, 1>& rhs )
{
    T res = lhs[0] * rhs[0];
    for(int i = 1; i<n; ++i)
        res += lhs[i] * rhs[i];
    return res;
}

/** dot product */
template<typename T, int n>
inline T dot( const Matrix<T, 1, n>& lhs,
              const Matrix<T, 1, n>& rhs )
{
    T res = lhs[0] * rhs[0];
    for(int i = 1; i<n; ++i)
        res += lhs[i] * rhs[i];
    return res;
}

/** cross product */
template<typename T>
inline Matrix<T, 3, 1> cross( const Matrix<T, 3, 1>& lhs,
                                  const Matrix<T, 3, 1>& rhs )
{
    return Matrix<T, 3, 1>( lhs.y * rhs.z - lhs.z * rhs.y,
                                lhs.z * rhs.x - lhs.x * rhs.z,
                                lhs.x * rhs.y - lhs.y * rhs.x );
}

/** cross product */
template<typename T>
inline Matrix<T, 1, 3> cross( const Matrix<T, 1, 3>& lhs,
                                  const Matrix<T, 1, 3>& rhs )
{
    return Matrix<T, 1, 3>( lhs.y * rhs.z - lhs.z * rhs.y,
                                lhs.z * rhs.x - lhs.x * rhs.z,
                                lhs.x * rhs.y - lhs.y * rhs.x );
}

/** vector length */
template<typename T, int n>
inline T length(const Matrix<T, n, 1>& vec)
{
    return ::sqrt( dot(vec, vec) );
}

/** vector length */
template<typename T, int n>
inline T length(const Matrix<T, 1, n>& vec)
{
    return ::sqrt( dot(vec, vec) );
}

/** normalize vector */
template<typename T, int n>
inline Matrix<T, n, 1> normalize(const Matrix<T, n, 1>& vec)
{
    return vec / length(vec);
}

/** normalize vector */
template<typename T, int n>
inline Matrix<T, 1, n> normalize(const Matrix<T, 1, n>& vec)
{
    return vec / length(vec);
}

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/** per component max */
inline Matrix<float, 4, 1> max(const Matrix<float, 4, 1>& a, const Matrix<float, 4, 1>& b)
{
    __m128 packed = _mm_max_ps(a.m128, b.m128);
    return Matrix<float, 4, 1>(packed);
}

/** per component max */
inline Matrix<float, 1, 4> max(const Matrix<float, 1, 4>& a, const Matrix<float, 1, 4>& b)
{
    __m128 packed = _mm_max_ps(a.m128, b.m128);
    return Matrix<float, 1, 4>(packed);
}

/** per component min */
inline Matrix<float, 4, 1> min(const Matrix<float, 4, 1>& a, const Matrix<float, 4, 1>& b)
{
    __m128 packed = _mm_min_ps(a.m128, b.m128);
    return Matrix<float, 4, 1>(packed);
}

/** per component min */
inline Matrix<float, 1, 4> min(const Matrix<float, 1, 4>& a, const Matrix<float, 1, 4>& b)
{
    __m128 packed = _mm_min_ps(a.m128, b.m128);
    return Matrix<float, 1, 4>(packed);
}

/** per component min & max*/
inline void min_max( const Matrix<float, 4, 1>& a,
                     const Matrix<float, 4, 1>& b,
                     Matrix<float, 4, 1>&       outMin,
                     Matrix<float, 4, 1>&       outMax )
{
    outMin.m128 = _mm_min_ps(a.m128, b.m128);
    outMax.m128 = _mm_max_ps(a.m128, b.m128);
}

/** per component min & max*/
inline void min_max( const Matrix<float, 1, 4>& a,
                     const Matrix<float, 1, 4>& b,
                     Matrix<float, 1, 4>&       outMin,
                     Matrix<float, 1, 4>&       outMax )
{
    outMin.m128 = _mm_min_ps(a.m128, b.m128);
    outMax.m128 = _mm_max_ps(a.m128, b.m128);
}

/** per component clamp */
inline Matrix<float, 4, 1> clamp(const Matrix<float, 4, 1>& vec, float minVal, float maxVal)
{
    __m128 valPacked   = _mm_set1_ps(minVal);
    __m128 clampPacked = _mm_max_ps(vec.m128, valPacked);
    valPacked          = _mm_set1_ps(maxVal);
    clampPacked        = _mm_min_ps(valPacked, clampPacked);

    return Matrix<float, 4, 1>(clampPacked);
}

/** per component clamp */
inline Matrix<float, 1, 4> clamp(const Matrix<float, 1, 4>& vec, float minVal, float maxVal)
{
    __m128 valPacked   = _mm_set1_ps(minVal);
    __m128 clampPacked = _mm_max_ps(vec.m128, valPacked);
    valPacked          = _mm_set1_ps(maxVal);
    clampPacked        = _mm_min_ps(valPacked, clampPacked);

    return Matrix<float, 1, 4>(clampPacked);
}

/** linear interpolation between vectors */
inline Matrix<float, 4, 1> lerp(const Matrix<float, 4, 1>& a, const Matrix<float, 4, 1>& b, float lerpVal)
{
    __m128 diffPacked = _mm_sub_ps(a.m128, b.m128);
    __m128 lerpPacked = _mm_set1_ps(lerpVal);

    __m128 resPacked  = _mm_mul_ps(diffPacked, lerpPacked);
    resPacked         = _mm_add_ps(a.m128, resPacked);

    return Matrix<float, 4, 1>(resPacked);
}

/** linear interpolation between vectors */
inline Matrix<float, 1, 4> lerp(const Matrix<float, 1, 4>& a, const Matrix<float, 1, 4>& b, float lerpVal)
{
    __m128 diffPacked = _mm_sub_ps(a.m128, b.m128);
    __m128 lerpPacked = _mm_set1_ps(lerpVal);

    __m128 resPacked  = _mm_mul_ps(diffPacked, lerpPacked);
    resPacked         = _mm_add_ps(a.m128, resPacked);

    return Matrix<float, 1, 4>(resPacked);
}

/** dot product */
inline float dot( const Matrix<float, 4, 1>& lhs,
                  const Matrix<float, 4, 1>& rhs )
{
    float res;

#ifdef SIMPLE_GL_USE_SSE4
    __m128 dotRes = _mm_dp_ps(lhs.m128, rhs.m128, 0xEE);
    _mm_mov_ss(&res, dotRes);
#elif defined(SIMPLE_GL_USE_SSE3)
    __m128 dotRes = _mm_mul_ps(lhs.m128, rhs.m128);
    dotRes        = _mm_hadd_ps(dotRes, dotRes);
    dotRes        = _mm_hadd_ps(dotRes, dotRes);
    _mm_store_ss(&res, dotRes);
#else // SSE2
    __m128 dotRes  = _mm_mul_ps(lhs.m128, rhs.m128);

    // [0][0] + [2][2], [1][1] + [3][3] , ... , ...
    __m128 shufled = _mm_movehl_ps(dotRes, dotRes);
    dotRes         = _mm_add_ps(dotRes, shufled);

    // [0][0] + [1][1] + [2][2] + [3][3], ... , ... , ...
    shufled        = _mm_movelh_ps(dotRes, dotRes);
    dotRes         = _mm_add_ps(dotRes, shufled);

    _mm_store_ss(&res, dotRes);
#endif

    return res;
}

/** dot product */
inline float dot( const Matrix<float, 1, 4>& lhs,
                  const Matrix<float, 1, 4>& rhs )
{
    float res;

#ifdef SIMPLE_GL_USE_SSE4
    __m128 dotRes = _mm_dp_ps(lhs.m128, rhs.m128, 0xEE);
    _mm_mov_ss(&res, dotRes);
#elif defined(SIMPLE_GL_USE_SSE3)
    __m128 dotRes = _mm_mul_ps(lhs.m128, rhs.m128);
    dotRes        = _mm_hadd_ps(dotRes, dotRes);
    dotRes        = _mm_hadd_ps(dotRes, dotRes);
    _mm_store_ss(&res, dotRes);
#else // SSE2
    __m128 dotRes  = _mm_mul_ps(lhs.m128, rhs.m128);

    // [0][0] + [2][2], [1][1] + [3][3] , ... , ...
    __m128 shufled = _mm_movehl_ps(dotRes, dotRes);
    dotRes         = _mm_add_ps(dotRes, shufled);

    // [0][0] + [1][1] + [2][2] + [3][3], ... , ... , ...
    shufled        = _mm_movelh_ps(dotRes, dotRes);
    dotRes         = _mm_add_ps(dotRes, shufled);

    _mm_store_ss(&res, dotRes);
#endif

    return res;
}

#endif // SIMPLE_GL_USE_SSE

SGL_END_MATH_NAMESPACE

#ifdef MSVC
#pragma warning( pop )
#endif

#endif // SIMPLE_GL_MATH_VECTOR_HPP
