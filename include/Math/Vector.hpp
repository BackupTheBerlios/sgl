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

#include <xmmintrin.h>
#ifdef SIMPLE_GL_USE_SSE4
#   include <smmintrin.h>
#elif defined(SIMPLE_GL_USE_SSE3)
#   include <pmmintrin.h>
#else
#   include <emmintrin.h>
#endif

#endif // __GNUC__

#endif // SIMPLE_GL_USE_SSE

#ifdef MSVC
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

// Forward
template<typename ValueType, int n, int m, INSTRUCTION_SET inst_set = DEFAULT_INSTRUCTION_SET>
class Matrix;

/** Vector column. */
template<typename ValueType, int n, INSTRUCTION_SET inst_set>
class Matrix<ValueType, n, 1, inst_set> :
    public MatrixBase<ValueType, n, 1, inst_set>
{
public:
    typedef MatrixBase<ValueType, n, 1, inst_set> base_type;
    typedef Matrix<ValueType, n, 1, inst_set>     this_type;

public:
    inline Matrix() {}
    inline Matrix(const this_type& vec) { std::copy(vec.arr, vec.arr + n, arr); }

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, n, 1, is>& vec)
    {
        std::copy(vec.arr, vec.arr + n, arr);
    }

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, n, 1, is>& vec)
    {
        std::copy(vec.arr, vec.arr + n, arr);
    }

    inline ValueType& operator [] (int i)
    {
        assert(i < n);
        return arr[i];
    }

    inline ValueType operator [] (int i) const
    {
        assert(i < n);
        return arr[i];
    }

public:
    ValueType arr[n];
};

/** Vector row. */
template<typename ValueType, int m, INSTRUCTION_SET inst_set>
class Matrix<ValueType, 1, m, inst_set> :
    public MatrixBase<ValueType, 1, m, inst_set>
{
public:
    typedef MatrixBase<ValueType, 1, m, inst_set> base_type;
    typedef Matrix<ValueType, 1, m, inst_set>     this_type;

public:
    inline Matrix() {}
    inline Matrix(const this_type& vec) { std::copy(vec.arr, vec.arr + m, arr); }

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, 1, m, is>& vec)
    {
        std::copy(vec.arr, vec.arr + m, arr);
    }

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 1, m, is>& vec)
    {
        std::copy(vec.arr, vec.arr + m, arr);
    }

    inline ValueType& operator [] (int i)
    {
        assert(i < m);
        return arr[i];
    }

    inline ValueType operator [] (int i) const
    {
        assert(i < m);
        return arr[i];
    }

public:
    ValueType arr[m];
};

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

template<>
class MatrixBase<float, 4, 1, SSE> :
    public sgl::Aligned<0x10>
{
public:
    typedef float       value_type;
    typedef int         matrix_tag;
    typedef int         vector_tag;

    static const int    num_elements = 4;
    static const int    num_rows     = 4;
    static const int    num_columns  = 1;

protected:
    /** Don't delete MatrixBase. */
    inline ~MatrixBase() {}
};

template<>
class MatrixBase<float, 1, 4, SSE> :
    public sgl::Aligned<0x10>
{
public:
    typedef float       value_type;
    typedef int         matrix_tag;
    typedef int         vector_tag;

    static const int    num_elements = 4;
    static const int    num_rows     = 1;
    static const int    num_columns  = 4;

protected:
    /** Don't delete MatrixBase. */
    inline ~MatrixBase() {}
};

/** Vector column. */
template<>
class Matrix<float, 4, 1, SSE> :
    public MatrixBase<float, 4, 1, SSE>
{
public:
    typedef MatrixBase<float, 4, 1, SSE>     base_type;
    typedef Matrix<float, 4, 1, SSE>         this_type;

public:
    inline Matrix() {}

    inline Matrix(__m128 sseReg) :
        m128(sseReg)
    {}

    inline Matrix(const this_type& vec) :
        m128(vec.m128)
    {
        m128 = _mm_set_ps(vec.w, vec.z, vec.y, vec.x);
    }

    inline Matrix(float _x, float _y, float _z, float _w)
    {
        m128 = _mm_set_ps(_w, _z, _y, _x);
    }

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<float, 4, 1, is>& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 4, 1, is>& vec) :
        x( static_cast<float>(vec.x) ),
        y( static_cast<float>(vec.y) ),
        z( static_cast<float>(vec.z) ),
        w( static_cast<float>(vec.w) )
    {}

    inline float& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    inline float operator [] (unsigned int i) const
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
class Matrix<float, 1, 4, SSE> :
    public MatrixBase<float, 1, 4, SSE>
{
public:
    typedef MatrixBase<float, 1, 4, SSE>     base_type;
    typedef Matrix<float, 1, 4, SSE>         this_type;

public:
    inline Matrix() {}

    inline Matrix(__m128 sseReg) :
        m128(sseReg)
    {}

    inline Matrix(const this_type& vec) :
        m128(vec.m128)
    {
        m128 = _mm_set_ps(vec.w, vec.z, vec.y, vec.x);
    }

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<float, 1, 4, is>& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

    inline Matrix(float _x, float _y, float _z, float _w)
    {
        m128 = _mm_set_ps(_w, _z, _y, _x);
    }

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 4, 1, is>& vec) :
        x( static_cast<float>(vec.x) ),
        y( static_cast<float>(vec.y) ),
        z( static_cast<float>(vec.z) ),
        w( static_cast<float>(vec.w) )
    {}

    inline float& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    inline float operator [] (unsigned int i) const
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
template<typename ValueType, INSTRUCTION_SET inst_set>
class Matrix<ValueType, 4, 1, inst_set> :
    public MatrixBase<ValueType, 4, 1, inst_set>
{
public:
    typedef MatrixBase<ValueType, 4, 1>     base_type;
    typedef Matrix<ValueType, 4, 1>         this_type;

public:
    inline Matrix() {}

    inline Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, 4, 1, is>& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

    inline Matrix(ValueType _x, ValueType _y, ValueType _z, ValueType _w) :
        x(_x),
        y(_y),
        z(_z),
        w(_w)
    {}

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 4, 1, is>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) ),
        w( static_cast<ValueType>(vec.w) )
    {}

    inline ValueType& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    inline ValueType operator [] (unsigned int i) const
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
template<typename ValueType, INSTRUCTION_SET inst_set>
class Matrix<ValueType, 1, 4, inst_set> :
    public MatrixBase<ValueType, 1, 4, inst_set>
{
public:
    typedef MatrixBase<ValueType, 1, 4, inst_set>     base_type;
    typedef Matrix<ValueType, 1, 4, inst_set>         this_type;

public:
    inline Matrix() {}

    inline Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, 1, 4, is>& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z),
        w(vec.w)
    {}

    inline Matrix(ValueType _x, ValueType _y, ValueType _z, ValueType _w) :
        x(_x),
        y(_y),
        z(_z),
        w(_w)
    {}

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 1, 4, is>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) ),
        w( static_cast<ValueType>(vec.w) )
    {}

    inline ValueType& operator [] (unsigned int i)
    {
        assert(i < 4);
        return arr[i];
    }

    inline ValueType operator [] (unsigned int i) const
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
template<typename ValueType, INSTRUCTION_SET inst_set>
class Matrix<ValueType, 3, 1, inst_set> :
    public MatrixBase<ValueType, 3, 1, inst_set>
{
public:
    typedef MatrixBase<ValueType, 3, 1, inst_set>     base_type;
    typedef Matrix<ValueType, 3, 1, inst_set>         this_type;

public:
    inline Matrix() {}

    inline Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z)
    {}

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, 3, 1, is>& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z)
    {}

    inline Matrix(ValueType _x, ValueType _y, ValueType _z) :
        x(_x),
        y(_y),
        z(_z)
    {}

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 3, 1, is>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) )
    {}

    inline ValueType& operator [] (unsigned int i)
    {
        assert(i < 3);
        return arr[i];
    }

    inline ValueType operator [] (unsigned int i) const
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
template<typename ValueType, INSTRUCTION_SET inst_set>
class Matrix<ValueType, 1, 3, inst_set> :
    public MatrixBase<ValueType, 1, 3, inst_set>
{
public:
    typedef MatrixBase<ValueType, 1, 3, inst_set>     base_type;
    typedef Matrix<ValueType, 1, 3, inst_set>         this_type;

public:
    inline Matrix() {}

    inline Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z)
    {}

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, 1, 3, is>& vec) :
        x(vec.x),
        y(vec.y),
        z(vec.z)
    {}

    inline Matrix(ValueType _x, ValueType _y, ValueType _z) :
        x(_x),
        y(_y),
        z(_z)
    {}

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 1, 3, is>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) ),
        z( static_cast<ValueType>(vec.z) )
    {}

    inline ValueType& operator [] (unsigned int i)
    {
        assert(i < 3);
        return arr[i];
    }

    inline ValueType operator [] (unsigned int i) const
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
template<typename ValueType, INSTRUCTION_SET inst_set>
class Matrix<ValueType, 2, 1, inst_set> :
    public MatrixBase<ValueType, 2, 1, inst_set>
{
public:
    typedef MatrixBase<ValueType, 2, 1, inst_set>     base_type;
    typedef Matrix<ValueType, 2, 1, inst_set>         this_type;

public:
    inline Matrix() {}

    inline Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y)
    {}

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, 2, 1, is>& vec) :
        x(vec.x),
        y(vec.y)
    {}

    inline Matrix(ValueType _x, ValueType _y) :
        x(_x),
        y(_y)
    {}

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 2, 1, is>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) )
    {}

    inline ValueType& operator [] (unsigned int i)
    {
        assert(i < 2);
        return arr[i];
    }

    inline ValueType operator [] (unsigned int i) const
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
template<typename ValueType, INSTRUCTION_SET inst_set>
class Matrix<ValueType, 1, 2, inst_set> :
    public MatrixBase<ValueType, 1, 2, inst_set>
{
public:
    typedef MatrixBase<ValueType, 1, 2, inst_set>     base_type;
    typedef Matrix<ValueType, 1, 2, inst_set>         this_type;

public:
    inline Matrix() {}

    inline Matrix(const this_type& vec) :
        x(vec.x),
        y(vec.y)
    {}

    template<INSTRUCTION_SET is>
    inline Matrix(const Matrix<ValueType, 1, 2, is>& vec) :
        x(vec.x),
        y(vec.y)
    {}

    inline Matrix(ValueType _x, ValueType _y) :
        x(_x),
        y(_y)
    {}

    template<typename T, INSTRUCTION_SET is>
    inline explicit Matrix(const Matrix<T, 1, 2, is>& vec) :
        x( static_cast<ValueType>(vec.x) ),
        y( static_cast<ValueType>(vec.y) )
    {}

    inline ValueType& operator [] (unsigned int i)
    {
        assert(i < 2);
        return arr[i];
    }

    inline ValueType operator [] (unsigned int i) const
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
typedef Matrix<double, 4, 1, FPU>           Vector4d;
typedef Matrix<double, 3, 1, FPU>           Vector3d;
typedef Matrix<double, 2, 1, FPU>           Vector2d;

typedef Matrix<float, 4, 1, FPU>            FPUVector4f;    /// Vector4f without SSE
typedef Matrix<float, 4, 1, SSE>            SSEVector4f;    /// Vector4f with SSE
#ifdef SIMPLE_GL_USE_SSE
typedef Matrix<float, 4, 1, SSE>            Vector4f;      
#else
typedef Matrix<float, 4, 1, FPU>            Vector4f;      
#endif 

typedef Matrix<float, 3, 1, FPU>            Vector3f;
typedef Matrix<float, 2, 1, FPU>            Vector2f;

typedef Matrix<int, 4, 1, FPU>              Vector4i;
typedef Matrix<int, 3, 1, FPU>              Vector3i;
typedef Matrix<int, 2, 1, FPU>              Vector2i;

typedef Matrix<unsigned int, 4, 1, FPU>     Vector4ui;
typedef Matrix<unsigned int, 3, 1, FPU>     Vector3ui;
typedef Matrix<unsigned int, 2, 1, FPU>     Vector2ui;

typedef Matrix<bool, 4, 1, FPU>             Vector4b;
typedef Matrix<bool, 3, 1, FPU>             Vector3b;
typedef Matrix<bool, 2, 1, FPU>             Vector2b;

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is>& operator *= (Matrix<T, n, 1, is>& lhs, const Matrix<T, n, 1, is>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs[i];
    return lhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is>& operator *= (Matrix<T, 1, n, is>& lhs, const Matrix<T, 1, n, is>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs[i];
    return lhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is>& operator *= (Matrix<T, n, 1, is>& lhs, T rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs;
    return lhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is>& operator *= (Matrix<T, 1, n, is>& lhs, T rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] *= rhs;
    return lhs;
}

/** div per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is>& operator /= (Matrix<T, n, 1, is>& lhs, const Matrix<T, n, 1, is>& rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= rhs[i];
    return lhs;
}

/** div per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is>& operator /= (Matrix<T, 1, n, is>& lhs, const Matrix<T, 1, n, is>& rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= rhs[i];
    return lhs;
}

/** div per component */
template<typename T, typename Y, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is>& operator /= (Matrix<T, n, 1, is>& lhs, Y rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= static_cast<T>(rhs);
    return lhs;
}

/** div per component */
template<typename T, typename Y, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is>& operator /= (Matrix<T, 1, n, is>& lhs, Y rhs)
{
    for(int i = 0; i<n; ++i)
        lhs[i] /= static_cast<T>(rhs);
    return lhs;
}

/** compare vectors */
template<typename T, int n, INSTRUCTION_SET is>
inline bool operator == (const Matrix<T, n, 1, is>& lhs, const Matrix<T, n, 1, is>& rhs)
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
template<typename T, int n, INSTRUCTION_SET is>
inline bool operator == (const Matrix<T, 1, n, is>& lhs, const Matrix<T, 1, n, is>& rhs)
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
template<typename T, int n, INSTRUCTION_SET is>
inline bool operator != (const Matrix<T, n, 1, is>& lhs, const Matrix<T, n, 1, is>& rhs)
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
template<typename T, int n, INSTRUCTION_SET is>
inline bool operator != (const Matrix<T, 1, n, is>& lhs, const Matrix<T, 1, n, is>& rhs)
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
inline Matrix<float, 4, 1, SSE>& operator += (Matrix<float, 4, 1, SSE>& lhs, const Matrix<float, 4, 1, SSE>& rhs)
{
    lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** add per component */
inline Matrix<float, 1, 4, SSE>& operator += (Matrix<float, 1, 4, SSE>& lhs, const Matrix<float, 1, 4, SSE>& rhs)
{
    lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** subtract per component */
inline Matrix<float, 4, 1, SSE>& operator -= (Matrix<float, 4, 1, SSE>& lhs, const Matrix<float, 4, 1, SSE>& rhs)
{
    lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** subtract per component */
inline Matrix<float, 1, 4, SSE>& operator -= (Matrix<float, 1, 4, SSE>& lhs, const Matrix<float, 1, 4, SSE>& rhs)
{
    lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** mul per component */
inline Matrix<float, 4, 1, SSE>& operator *= (Matrix<float, 4, 1, SSE>& lhs, const Matrix<float, 4, 1, SSE>& rhs)
{
    lhs.m128 = _mm_mul_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** mul per component */
inline Matrix<float, 1, 4, SSE>& operator *= (Matrix<float, 1, 4, SSE>& lhs, const Matrix<float, 1, 4, SSE>& rhs)
{
    lhs.m128 = _mm_mul_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** mul per component */
inline Matrix<float, 4, 1, SSE>& operator *= (Matrix<float, 4, 1, SSE>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128 = _mm_mul_ps(lhs.m128, packed);
    return lhs;
}

/** mul per component */
inline Matrix<float, 1, 4, SSE>& operator *= (Matrix<float, 1, 4, SSE>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128 = _mm_mul_ps(lhs.m128, packed);
    return lhs;
}

/** div per component */
inline Matrix<float, 4, 1, SSE>& operator /= (Matrix<float, 4, 1, SSE>& lhs, const Matrix<float, 4, 1, SSE>& rhs)
{
    lhs.m128 = _mm_div_ps(lhs.m128, rhs.m128);
    return lhs;
}

/** div per component */
inline Matrix<float, 1, 4, SSE>& operator /= (Matrix<float, 1, 4, SSE>& lhs, const Matrix<float, 1, 4, SSE>& rhs)
{
    lhs.m128 = _mm_div_ps(lhs.m128, rhs.m128);
    return lhs;
}

#endif // SIMPLE_GL_USE_SSE

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> operator * (const Matrix<T, n, 1, is>& lhs, const Matrix<T, n, 1, is>& rhs)
{
    Matrix<T, n, 1, is> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> operator * (const Matrix<T, 1, n, is>& lhs, const Matrix<T, 1, n, is>& rhs)
{
    Matrix<T, 1, n, is> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> operator * (const Matrix<T, n, 1, is>& lhs, T rhs)
{
    Matrix<T, n, 1, is> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> operator * (const Matrix<T, 1, n, is>& lhs, T rhs)
{
    Matrix<T, 1, n, is> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> operator * (T lhs, const Matrix<T, n, 1, is>& rhs)
{
    Matrix<T, n, 1, is> tmp(rhs);
    return tmp *= lhs;
}

/** mul per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> operator * (T lhs, const Matrix<T, 1, n, is>& rhs)
{
    Matrix<T, 1, n, is> tmp(rhs);
    return tmp *= lhs;
}

/** mul matrix per vector column */
template<typename T, int n, int m, INSTRUCTION_SET is>
Matrix<T, n, 1, is> operator * (const Matrix<T, n, m, is>& lhs, const Matrix<T, m, 1, is>& rhs)
{
    Matrix<T, n, 1, is> res;
    for(int i = 0; i<n; ++i)
    {
        res[i] = lhs[i][0] * rhs[0];
        for(int k = 1; k<m; ++k)
            res[i] += lhs[i][k] * rhs[k];
    }
    return res;
}

/** mul matrix44 per vector4 */
template<typename T, INSTRUCTION_SET is>
Matrix<T, 4, 1, is> operator * (const Matrix<T, 4, 4, is>& lhs, const Matrix<T, 4, 1, is>& rhs)
{
    Matrix<T, 4, 1, is> res;
    res[0] = lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2] + lhs[0][3] * rhs[3];
    res[1] = lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2] + lhs[1][3] * rhs[3];
    res[2] = lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2] + lhs[2][3] * rhs[3];
    res[3] = lhs[3][0] * rhs[0] + lhs[3][1] * rhs[1] + lhs[3][2] * rhs[2] + lhs[3][3] * rhs[3];
    return res;
}

/** mul matrix44 per vector3 */
template<typename T, INSTRUCTION_SET is>
Matrix<T, 3, 1, is> operator * (const Matrix<T, 4, 4, is>& lhs, const Matrix<T, 3, 1, is>& rhs)
{
    Matrix<T, 3, 1, is> res;
    res[0] = lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2];
    res[1] = lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2];
    res[2] = lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2];
    return res;
}

/** mul column per row */
template<typename T, int n, int l, INSTRUCTION_SET is>
Matrix<T, n, l, is> operator * (const Matrix<T, n, 1, is>& lhs, const Matrix<T, 1, l, is>& rhs)
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
template<typename T,int m, int l, INSTRUCTION_SET is>
Matrix<T, 1, l, is> operator * (const Matrix<T, 1, m, is>& lhs, const Matrix<T, m, l, is>& rhs)
{
    Matrix<T, 1, l, is> res;
    for(int j = 0; j<l; ++j)
    {
        res[j] = lhs[0] * rhs[0][j];
        for(int k = 1; k<m; ++k)
            res[j] += lhs[k] * rhs[k][j];
    }
    return res;
}

/** div per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> operator / (const Matrix<T, n, 1, is>& lhs, const Matrix<T, n, 1, is>& rhs)
{
    Matrix<T, n, 1, is> tmp(lhs);
    return tmp /= rhs;
}

/** div per component */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> operator / (const Matrix<T, 1, n, is>& lhs, const Matrix<T, 1, n, is>& rhs)
{
    Matrix<T, 1, n, is> tmp(lhs);
    return tmp /= rhs;
}

/** div per component */
template<typename T, typename Y, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> operator / (const Matrix<T, n, 1, is>& lhs, Y rhs)
{
    Matrix<T, n, 1, is> tmp(lhs);
    return tmp /= rhs;
}

/** div per component */
template<typename T, typename Y, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> operator / (const Matrix<T, 1, n, is>& lhs, Y rhs)
{
    Matrix<T, 1, n, is> tmp(lhs);
    return tmp /= rhs;
}

/** print vector */
template<typename T, int n, INSTRUCTION_SET is>
std::ostream& operator << (std::ostream& os, const Matrix<T, n, 1, is>& vec)
{
    for(int i = 0; i<n-1; ++i) {
        os << vec[i] << ' ';
    }

    return os << vec[n-1];
}

/** print vector */
template<typename T, int n, INSTRUCTION_SET is>
std::ostream& operator << (std::ostream& os, const Matrix<T, 1, n, is>& vec)
{
    for(int i = 0; i<n-1; ++i) {
        os << vec[i] << ' ';
    }

    return os << vec[n-1];
}

/** read vector */
template<typename T, int n, INSTRUCTION_SET is>
std::istream& operator >> (std::istream& istream, Matrix<T, n, 1, is>& vec)
{
    for(int i = 0; i<n; ++i) {
        istream >> vec[i];
    }

    return istream;
}

/** read vector */
template<typename T, int n, INSTRUCTION_SET is>
std::istream& operator >> (std::istream& istream, Matrix<T, 1, n, is>& vec)
{
    for(int i = 0; i<n; ++i) {
        istream >> vec[i];
    }

    return istream;
}

/** append value to vector */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n+1, 1, is> make_vec(const Matrix<T, n, 1, is>& vec, T val)
{
    Matrix<T, n+1, 1, is> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n] = val;
    return res;
}

/** append value to vector */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n+1, is> make_vec(const Matrix<T, 1, n, is>& vec, T val)
{
    Matrix<T, 1, n+1, is> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n] = val;
    return res;
}

/** append 2 values to vector */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n+2, 1, is> make_vec(const Matrix<T, n, 1, is>& vec, T a, T b)
{
    Matrix<T, n+2, 1, is> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n]   = a;
    res[n+1] = b;
    return res;
}

/** append 2 values to vector */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n+2, is> make_vec(const Matrix<T, 1, n, is>& vec, T a, T b)
{
    Matrix<T, 1, n+2, is> res;
    std::copy(vec.arr, vec.arr + n, res.arr);
    res[n]   = a;
    res[n+1] = b;
    return res;
}

/** concatenate vectors */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n+m, 1, is> make_vec(const Matrix<T, n, 1, is>& a, const Matrix<T, m, 1, is>& b)
{
    Matrix<T, n+m, 1, is> res;
    std::copy(a.arr, a.arr + n, res.arr);
    std::copy(b.arr, b.arr + m, res.arr + n);
    return res;
}

/** concatenate vectors */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, 1, n+m, is> make_vec(const Matrix<T, 1, n, is>& a, const Matrix<T, 1, m, is>& b)
{
    Matrix<T, 1, n+m, is> res;
    std::copy(a.arr, a.arr + n, res.arr);
    std::copy(b.arr, b.arr + m, res.arr + n);
    return res;
}


/** compare to vectors using threshold */
template<typename T, int n, INSTRUCTION_SET is>
inline bool equal( const Matrix<T, n, 1, is>& a,
                   const Matrix<T, n, 1, is>& b,
                   T threshold = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( abs(a[i] - b[i]) >= threshold ) {
            return false;
        }
    }

    return true;
}

/** compare to vectors using threshold */
template<typename T, int n, INSTRUCTION_SET is>
inline bool equal( const Matrix<T, 1, n, is>& a,
                   const Matrix<T, 1, n, is>& b,
                   T threshold = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( abs(a[i] - b[i]) >= threshold ) {
            return false;
        }
    }

    return true;
}

/** per component max */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> max(const Matrix<T, n, 1, is>& a, const Matrix<T, n, 1, is>& b)
{
    Matrix<T, n, 1, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::max(a[i], b[i]);
    }
    return result;
}

/** per component max */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> max(const Matrix<T, 1, n, is>& a, const Matrix<T, 1, n, is>& b)
{
    Matrix<T, 1, n, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::max(a[i], b[i]);
    }
    return result;
}

/** per component min */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> min(const Matrix<T, n, 1, is>& a, const Matrix<T, n, 1, is>& b)
{
    Matrix<T, n, 1, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min(a[i], b[i]);
    }
    return result;
}

/** per component min */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> min(const Matrix<T, 1, n, is>& a, const Matrix<T, 1, n, is>& b)
{
    Matrix<T, 1, n, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min(a[i], b[i]);
    }
    return result;
}

/** per component min & max*/
template<typename T, int n, INSTRUCTION_SET is>
inline void min_max( const Matrix<T, n, 1, is>& a,
                     const Matrix<T, n, 1, is>& b,
                     Matrix<T, n, 1, is>&       outMin,
                     Matrix<T, n, 1, is>&       outMax )
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
template<typename T, int n, INSTRUCTION_SET is>
inline void min_max( const Matrix<T, 1, n, is>& a,
                     const Matrix<T, 1, n, is>& b,
                     Matrix<T, 1, n, is>&       outMin,
                     Matrix<T, 1, n, is>&       outMax )
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
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> clamp(const Matrix<T, n, 1, is>& vec, T minVal, T maxVal)
{
    Matrix<T, n, 1, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min( std::max(vec[i], minVal), maxVal );
    }
    return result;
}

/** per component clamp */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> clamp(const Matrix<T, 1, n, is>& vec, T minVal, T maxVal)
{
    Matrix<T, 1, n, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min( std::max(vec[i], minVal), maxVal );
    }
    return result;
}

/** linear interpolation between vectors */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> lerp(const Matrix<T, n, 1, is>& a, const Matrix<T, n, 1, is>& b, T lerpVal)
{
    Matrix<T, n, 1, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = a[i] + lerpVal * (b[i] - a[i]);
    }
    return result;
}

/** linear interpolation between vectors */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> lerp(const Matrix<T, 1, n, is>& a, const Matrix<T, 1, n, is>& b, T lerpVal)
{
    Matrix<T, 1, n, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = a[i] + lerpVal * (b[i] - a[i]);
    }
    return result;
}

/** equal to slice<0, 3>
 * TODO: Add check fo n
 * @see slice
 */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 3, 1, is> xyz(const Matrix<T, n, 1, is>& vec)
{
    return Matrix<T, 3, 1, is>(vec.x, vec.y, vec.z);
}

/** equal to slice<0, 3>
 * TODO: Add check fo n
 * @see slice
 */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, 3, is> xyz(const Matrix<T, 1, n, is>& vec)
{
    return Matrix<T, 1, 3, is>(vec.x, vec.y, vec.z);
}

/** equal to slice<0, 2>
 * TODO: Add check fo n
 * @see slice
 */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 2, 1, is> xy(const Matrix<T, n, 1, is>& vec)
{
    return Matrix<T, 2, 1, is>(vec.x, vec.y);
}

/** equal to slice<0, 2>
 * TODO: Add check fo n
 * @see slice
 */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, 2, is> xy(const Matrix<T, 1, n, is>& vec)
{
    return Matrix<T, 1, 2, is>(vec.x, vec.y);
}

/** dot product */
template<typename T, int n, INSTRUCTION_SET is>
inline T dot( const Matrix<T, n, 1, is>& lhs,
              const Matrix<T, n, 1, is>& rhs )
{
    T res = lhs[0] * rhs[0];
    for(int i = 1; i<n; ++i)
        res += lhs[i] * rhs[i];
    return res;
}

/** dot product */
template<typename T, int n, INSTRUCTION_SET is>
inline T dot( const Matrix<T, 1, n, is>& lhs,
              const Matrix<T, 1, n, is>& rhs )
{
    T res = lhs[0] * rhs[0];
    for(int i = 1; i<n; ++i)
        res += lhs[i] * rhs[i];
    return res;
}

/** cross product */
template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 3, 1, is> cross( const Matrix<T, 3, 1, is>& lhs,
                                  const Matrix<T, 3, 1, is>& rhs )
{
    return Matrix<T, 3, 1, is>( lhs.y * rhs.z - lhs.z * rhs.y,
                                lhs.z * rhs.x - lhs.x * rhs.z,
                                lhs.x * rhs.y - lhs.y * rhs.x );
}

/** cross product */
template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 1, 3, is> cross( const Matrix<T, 1, 3, is>& lhs,
                                  const Matrix<T, 1, 3, is>& rhs )
{
    return Matrix<T, 1, 3, is>( lhs.y * rhs.z - lhs.z * rhs.y,
                                lhs.z * rhs.x - lhs.x * rhs.z,
                                lhs.x * rhs.y - lhs.y * rhs.x );
}

/** vector length */
template<typename T, int n, INSTRUCTION_SET is>
inline T length(const Matrix<T, n, 1, is>& vec)
{
    return ::sqrt( dot(vec, vec) );
}

/** vector length */
template<typename T, int n, INSTRUCTION_SET is>
inline T length(const Matrix<T, 1, n, is>& vec)
{
    return ::sqrt( dot(vec, vec) );
}

/** normalize vector */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, 1, is> normalize(const Matrix<T, n, 1, is>& vec)
{
    return vec / length(vec);
}

/** normalize vector */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, 1, n, is> normalize(const Matrix<T, 1, n, is>& vec)
{
    return vec / length(vec);
}

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/** per component max */
inline Matrix<float, 4, 1, SSE> max(const Matrix<float, 4, 1, SSE>& a, const Matrix<float, 4, 1, SSE>& b)
{
    __m128 packed = _mm_max_ps(a.m128, b.m128);
    return Matrix<float, 4, 1, SSE>(packed);
}

/** per component max */
inline Matrix<float, 1, 4, SSE> max(const Matrix<float, 1, 4, SSE>& a, const Matrix<float, 1, 4, SSE>& b)
{
    __m128 packed = _mm_max_ps(a.m128, b.m128);
    return Matrix<float, 1, 4, SSE>(packed);
}

/** per component min */
inline Matrix<float, 4, 1, SSE> min(const Matrix<float, 4, 1, SSE>& a, const Matrix<float, 4, 1, SSE>& b)
{
    __m128 packed = _mm_min_ps(a.m128, b.m128);
    return Matrix<float, 4, 1, SSE>(packed);
}

/** per component min */
inline Matrix<float, 1, 4, SSE> min(const Matrix<float, 1, 4, SSE>& a, const Matrix<float, 1, 4, SSE>& b)
{
    __m128 packed = _mm_min_ps(a.m128, b.m128);
    return Matrix<float, 1, 4, SSE>(packed);
}

/** per component min & max*/
inline void min_max( const Matrix<float, 4, 1, SSE>& a,
                     const Matrix<float, 4, 1, SSE>& b,
                     Matrix<float, 4, 1, SSE>&       outMin,
                     Matrix<float, 4, 1, SSE>&       outMax )
{
    outMin.m128 = _mm_min_ps(a.m128, b.m128);
    outMax.m128 = _mm_max_ps(a.m128, b.m128);
}

/** per component min & max*/
inline void min_max( const Matrix<float, 1, 4, SSE>& a,
                     const Matrix<float, 1, 4, SSE>& b,
                     Matrix<float, 1, 4, SSE>&       outMin,
                     Matrix<float, 1, 4, SSE>&       outMax )
{
    outMin.m128 = _mm_min_ps(a.m128, b.m128);
    outMax.m128 = _mm_max_ps(a.m128, b.m128);
}

/** per component clamp */
inline Matrix<float, 4, 1, SSE> clamp(const Matrix<float, 4, 1, SSE>& vec, float minVal, float maxVal)
{
    __m128 valPacked   = _mm_set1_ps(minVal);
    __m128 clampPacked = _mm_max_ps(vec.m128, clampPacked);
    valPacked          = _mm_set1_ps(maxVal);
    clampPacked        = _mm_min_ps(vec.m128, clampPacked);

    return Matrix<float, 4, 1, SSE>(clampPacked);
}

/** per component clamp */
inline Matrix<float, 1, 4, SSE> clamp(const Matrix<float, 1, 4, SSE>& vec, float minVal, float maxVal)
{
    __m128 valPacked   = _mm_set1_ps(minVal);
    __m128 clampPacked = _mm_max_ps(vec.m128, clampPacked);
    valPacked          = _mm_set1_ps(maxVal);
    clampPacked        = _mm_min_ps(vec.m128, clampPacked);

    return Matrix<float, 1, 4, SSE>(clampPacked);
}

/** linear interpolation between vectors */
inline Matrix<float, 4, 1, SSE> lerp(const Matrix<float, 4, 1, SSE>& a, const Matrix<float, 4, 1, SSE>& b, float lerpVal)
{
    __m128 diffPacked = _mm_sub_ps(a.m128, b.m128);
    __m128 lerpPacked = _mm_set1_ps(lerpVal);

    __m128 resPacked  = _mm_mul_ps(diffPacked, lerpPacked);
    resPacked         = _mm_add_ps(a.m128, resPacked);

    return Matrix<float, 4, 1, SSE>(resPacked);
}

/** linear interpolation between vectors */
inline Matrix<float, 1, 4, SSE> lerp(const Matrix<float, 1, 4, SSE>& a, const Matrix<float, 1, 4, SSE>& b, float lerpVal)
{
    __m128 diffPacked = _mm_sub_ps(a.m128, b.m128);
    __m128 lerpPacked = _mm_set1_ps(lerpVal);

    __m128 resPacked  = _mm_mul_ps(diffPacked, lerpPacked);
    resPacked         = _mm_add_ps(a.m128, resPacked);

    return Matrix<float, 1, 4, SSE>(resPacked);
}

/** dot product */
inline float dot( const Matrix<float, 4, 1, SSE>& lhs,
                  const Matrix<float, 4, 1, SSE>& rhs )
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
inline float dot( const Matrix<float, 1, 4, SSE>& lhs,
                  const Matrix<float, 1, 4, SSE>& rhs )
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

// TODO: cross product may need to be optimized
// Length is calculated using dot
// Normalize is calculated using length

#endif // SIMPLE_GL_USE_SSE

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#ifdef MSVC
#pragma warning( pop )
#endif

#endif // SIMPLE_GL_MATH_VECTOR_HPP
