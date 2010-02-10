#ifndef GRAPHICS_MATH_QUATERNION_HPP
#define GRAPHICS_MATH_QUATERNION_HPP

#include "Matrix.hpp"

#ifdef SGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

template<typename T, INSTRUCTION_SET inst_set = DEFAULT_INSTRUCTION_SET>
class Quaternion
{
public:
    Quaternion() {}

    Quaternion(const Quaternion& rhs) :
        x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

    template<INSTRUCTION_SET is>
    Quaternion(const Quaternion<T, is>& rhs) :
        x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

    template<INSTRUCTION_SET is>
    Quaternion(const Matrix<T, 3, 1, is>& vec, T _w) :
        x(vec.x), y(vec.y), z(vec.z), w(_w) {}

    Quaternion(T _x, T _y, T _z, T _w) :
        x(_x), y(_y), z(_z), w(_w) {}

    template<INSTRUCTION_SET is>
    explicit Quaternion(const Matrix<T, 4, 1, is>& vec) :
        x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

    // operators
    T& operator [] (int i)
    {
        assert(i < 4);
        return arr[i];
    }

    T operator [] (int i) const
    {
        assert(i < 4);
        return arr[i];
    }

public:
    union
    {
        struct {
            T x,y,z,w;
        };
        T arr[4];
    };
};

#ifdef SIMPLE_GL_USE_SSE

template<>
class Quaternion<float, SSE> :
    public sgl::Aligned<0x10>
{
public:
    typedef float               value_type;
    typedef Quaternion<float>   this_type;

public:
    Quaternion() {}

    Quaternion(const Quaternion& rhs) :
        x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

    template<INSTRUCTION_SET is>
    Quaternion(const Matrix<float, 3, 1, is>& vec, float _w) :
        x(vec.x), y(vec.y), z(vec.z), w(_w) {}

    Quaternion(float _x, float _y, float _z, float _w) :
        x(_x), y(_y), z(_z), w(_w) {}
    
    explicit Quaternion(const __m128& _m128) :
        m128(_m128)
    {}

    template<INSTRUCTION_SET is>
    explicit Quaternion(const Matrix<float, 4, 1, is>& vec) :
        x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

    explicit Quaternion(const Matrix<float, 4, 1, SSE>& vec) :
        m128(vec.m128) {}

    // operators
    inline float& operator [] (int i)
    {
        assert(i < 4);
        return arr[i];
    }

    inline float operator [] (int i) const
    {
        assert(i < 4);
        return arr[i];
    }

public:
    union
    {
        __m128 m128;
        struct {
            float x,y,z,w;
        };
        float arr[4];
    };
};

#endif // SIMPLE_GL_USE_SSE

template<typename T, INSTRUCTION_SET i>
inline bool operator == (const Quaternion<T, i>& lhs, const Quaternion<T, i>& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w);
}

template<typename T, INSTRUCTION_SET i>
inline bool operator != (const Quaternion<T, i>& lhs, const Quaternion<T, i>& rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y) || (lhs.z != rhs.z) || (lhs.w != rhs.w);
}

template<typename T, INSTRUCTION_SET i>
inline Quaternion<T, i> operator - (const Quaternion<T, i>& rhs)
{
    return Quaternion<T, i>(-rhs.x, -rhs.y, -rhs.z, -rhs.w);
}

template<typename T, INSTRUCTION_SET i>
inline Quaternion<T, i>& operator += (Quaternion<T, i>& lhs, const Quaternion<T, i>& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}

template<typename T, INSTRUCTION_SET i>
inline Quaternion<T, i>& operator -= (Quaternion<T, i>& lhs, const Quaternion<T, i>& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}

template<typename T, INSTRUCTION_SET i>
inline Quaternion<T, i>& operator *= (Quaternion<T, i>& lhs, const Quaternion<T, i>& rhs)
{
    T tmpX = lhs.x;
    T tmpY = lhs.y;
    T tmpZ = lhs.z;

    lhs.x = lhs.w * rhs.x + tmpX * rhs.w + tmpY * rhs.z - tmpZ * rhs.y;
    lhs.y = lhs.w * rhs.y + tmpY * rhs.w + tmpZ * rhs.y - tmpX * rhs.z;
    lhs.z = lhs.w * rhs.z + tmpZ * rhs.w + tmpX * rhs.y - tmpY * rhs.x;
    lhs.w = lhs.w * rhs.w - tmpX * rhs.x - tmpY * rhs.y - tmpZ * rhs.z;

    return lhs;
}

template<typename T, INSTRUCTION_SET i>
inline Quaternion<T, i>& operator *= (Quaternion<T, i>& lhs, T rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    lhs.w *= rhs;
    return lhs;
}

template<typename T, INSTRUCTION_SET i>
inline Matrix<T, 3, 1, FPU> operator *= (Matrix<T, 3, 1, FPU>& lhs, const Quaternion<T, i>& rhs)
{
    return to_matrix(rhs) * lhs;
}

template<typename T, INSTRUCTION_SET i>
inline Quaternion<T, i>& operator /= (Quaternion<T, i>& lhs, T rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    lhs.w /= rhs;
    return lhs;
}

// functions
template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 4, 1, is> as_vec(const Quaternion<T, is>& quat)
{
    return Matrix<T, 4, 1, is>(quat.x, quat.y, quat.z, quat.w);
}

template<typename T, INSTRUCTION_SET is>
inline Quaternion<T, is> conjugate(const Quaternion<T, is>& quat)
{
    return Quaternion<T, is>(-quat.x, -quat.y, -quat.z, quat.w);
}

template<typename T, INSTRUCTION_SET is>
inline Quaternion<T, is> invert(const Quaternion<T, is>& quat)
{
    return conjugate(quat) / norm(quat);
}

template<typename T, INSTRUCTION_SET is>
inline T norm(const Quaternion<T, is>& quat)
{
    return quat.x*quat.x + quat.y*quat.y + quat.z*quat.z + quat.w*quat.w;
}

template<typename T, INSTRUCTION_SET is>
inline T magnitude(const Quaternion<T, is>& quat)
{
    return sqrt(quat.x*quat.x + quat.y*quat.y + quat.z*quat.z + quat.w*quat.w);
}

/** normalize vector */
template<typename T, INSTRUCTION_SET is>
inline Quaternion<T, is> normalize(const Quaternion<T, is>& quat)
{
    return quat / magnitude(quat);
}

template<typename T, int n, INSTRUCTION_SET is>
inline Quaternion<T, is> from_matrix(const Matrix<T, n, n, is>& m)
{
    Quaternion<T, is> quat;

    T tr = m[0][0] + m[1][1] + m[2][2]; // trace of martix
    if (tr > 0.0)
    {
        // if trace positive than "w" is biggest component
        Quaternion<T, is> quat( m[1][2] - m[2][1],
                                m[2][0] - m[0][2],
                                m[0][1] - m[1][0],
                                tr + T(1.0) );
        return quat * T(0.5 / sqrt(quat.w));
    }
    else if ( (m[0][0] > m[1][1]) && (m[0][0] > m[2][2]) )
    {
        // Some of vector components is bigger
        Quaternion<T, is> quat( T(1.0) + m[0][0] - m[1][1] - m[2][2],
                                m[1][0] + m[0][1],
                                m[2][0] + m[0][2],
                                m[1][2] - m[2][1] );
        return quat * T(0.5 / sqrt(quat.x));

    }
    else if ( m[1][1] > m[2][2] )
    {
        Quaternion<T, is> quat( m[1][0] + m[0][1],
                                T(1.0) + m[1][1] - m[0][0] - m[2][2],
                                m[2][1] + m[1][2],
                                m[2][0] - m[0][2] );
        return quat * T(0.5 / sqrt(quat.y));

    }
    else
    {
        Quaternion<T, is> quat( m[2][0] + m[0][2],
                                m[2][1] + m[1][2],
                                T(1.0) + m[2][2] - m[0][0] - m[1][1],
                                m[0][1] - m[1][0] );
        return quat * T(0.5 / sqrt(quat.z));
    }
}

template<typename T, INSTRUCTION_SET i>
inline Matrix<T, 3, 3, FPU> to_matrix(const Quaternion<T, i>& quat)
{
    T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = quat.x * static_cast<T>(2);
    y2 = quat.y * static_cast<T>(2);
    z2 = quat.z * static_cast<T>(2);

    xx = quat.x * x2;
    xy = quat.x * y2;
    xz = quat.x * z2;

    yy = quat.y * y2;
    yz = quat.y * z2;
    zz = quat.z * z2;

    wx = quat.w * x2;
    wy = quat.w * y2;
    wz = quat.w * z2;

    Matrix<T, 3, 3, FPU> matrix;
    matrix[0][0] = static_cast<T>(1) - (yy + zz);
    matrix[1][0] = xy - wz;
    matrix[2][0] = xz + wy;

    matrix[0][1] = xy + wz;
    matrix[1][1] = static_cast<T>(1) - (xx + zz);
    matrix[2][1] = yz - wx;

    matrix[0][2] = xz - wy;
    matrix[1][2] = yz + wx;
    matrix[2][2] = static_cast<T>(1) - (xx + yy);

    return matrix;
}

template<typename T, INSTRUCTION_SET i>
inline Matrix<T, 4, 4, i> to_matrix_4x4(const Quaternion<T, i>& quat)
{
    T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = quat.x * static_cast<T>(2);
    y2 = quat.y * static_cast<T>(2);
    z2 = quat.z * static_cast<T>(2);

    xx = quat.x * x2;
    xy = quat.x * y2;
    xz = quat.x * z2;

    yy = quat.y * y2;
    yz = quat.y * z2;
    zz = quat.z * z2;

    wx = quat.w * x2;
    wy = quat.w * y2;
    wz = quat.w * z2;

    Matrix<T, 4, 4, i> matrix;
    matrix[0][0] = static_cast<T>(1) - (yy + zz);
    matrix[1][0] = xy - wz;
    matrix[2][0] = xz + wy;
    matrix[3][0] = T(0.0);

    matrix[0][1] = xy + wz;
    matrix[1][1] = static_cast<T>(1) - (xx + zz);
    matrix[2][1] = yz - wx;
    matrix[3][1] = T(0.0);

    matrix[0][2] = xz - wy;
    matrix[1][2] = yz + wx;
    matrix[2][2] = static_cast<T>(1) - (xx + yy);
    matrix[3][2] = T(0.0);

    matrix[0][3] = T(0.0);
    matrix[1][3] = T(0.0);
    matrix[2][3] = T(0.0);
    matrix[3][3] = T(1.0);

    return matrix;
}

template<typename T>
inline Quaternion<T, DEFAULT_INSTRUCTION_SET> from_axis_angle(const Matrix<T, 3, 1, FPU>& axis)
{
    T angle = length(axis);
    return Quaternion<T, DEFAULT_INSTRUCTION_SET>( axis / angle * sin(angle/2), cos(angle/2) );
}

template<typename T, INSTRUCTION_SET i>
inline Matrix<T, 3, 1, FPU> to_axis_angle(const Quaternion<T, i>& quat)
{
    Matrix<T, 3, 1, i> axis(quat.x, quat.y, quat.z);
    T vl  = length(axis);
    axis /= vl;
    if( quat.w < 0 ) {
        axis *= T( 2.0 * atan2(-vl, -quat.w) ); //-PI,0
    }
    else {
        axis *= T( 2.0 * atan2( vl,  quat.w) ); //0,PI
    }
}

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/*
template<>
bool Quaternion<float>::operator == (const Quaternion<float>& rhs) const
*/

/*
template<>
bool Quaternion<float>::operator != (const Quaternion<float>& rhs) const
*/

template<>
inline Quaternion<float, SSE> operator - (const Quaternion<float, SSE>& rhs)
{
    __m128 zero    = _mm_set1_ps(0.0f);
    __m128 invQuat = _mm_sub_ps(zero, rhs.m128);
    return Quaternion<float, SSE>(invQuat);
}

template<>
inline Quaternion<float, SSE>& operator += (Quaternion<float, SSE>& lhs, const Quaternion<float, SSE>& rhs)
{
    lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
    return lhs;
}

template<>
inline Quaternion<float, SSE>& operator -= (Quaternion<float, SSE>& lhs, const Quaternion<float, SSE>& rhs)
{
    lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
    return lhs;
}
/*
template<>
inline Quaternion<float, SSE>& operator *= (Quaternion<float, SSE>& lhs, const Quaternion<float, SSE>& rhs)
{
    float tmpX = lhs.x;
    float tmpY = lhs.y;
    float tmpZ = lhs.z;

    lhs.x = tmpX * rhs.x - tmpY  * rhs.y - tmpZ  * rhs.z - lhs.w * rhs.w;
    lhs.y = tmpX * rhs.y - tmpY  * rhs.x - tmpZ  * rhs.w - lhs.w * rhs.z;
    lhs.z = tmpX * rhs.z - tmpZ  * rhs.x - lhs.w * rhs.y - tmpY  * rhs.w;
    lhs.w = tmpX * rhs.w - lhs.w * rhs.x - tmpY  * rhs.z - tmpZ  * rhs.y;

    return lhs;
}
*/
template<>
inline Quaternion<float, SSE>& operator *= (Quaternion<float, SSE>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128      = _mm_mul_ps(lhs.m128, packed);
    return lhs;
}

template<>
inline Quaternion<float, SSE>& operator /= (Quaternion<float, SSE>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128      = _mm_div_ps(lhs.m128, packed);
    return lhs;
}

template<typename T>
inline Matrix<float, 4, 1, SSE> as_vec(const Quaternion<float, SSE>& quat)
{
    return Matrix<float, 4, 1, SSE>(quat.m128);
}


/*
template<>
Matrix<float, 3, 3> to_matrix(const Quaternion<float>& quat) const
*/

/*
template<>
Matrix<float, 4, 4> to_matrix_4x4(const Quaternion<float>& quat) const
*/

/*
template<>
void to_axis_angle(const Quaternion<float>& quat, Matrix<float, 3, 1>& axis, float& angle) const
*/

#endif // SIMPLE_GL_USE_SSE

// standart quaternions

#ifdef SIMPLE_GL_USE_SSE
typedef Quaternion<float, SSE>      Quaternionf;    /// Quaternion with SSE
#else
typedef Quaternion<float, FPU>      Quaternionf;    /// Quaternion with default instruction set
#endif 
typedef Quaternion<double>          Quaterniond;

template<typename T, INSTRUCTION_SET i>
inline math::Quaternion<T, i> operator + (const math::Quaternion<T, i>& lhs, const math::Quaternion<T, i>& rhs)
{
    math::Quaternion<T, i> tmp(lhs);
    return tmp += rhs;
}

template<typename T, INSTRUCTION_SET i>
inline math::Quaternion<T, i> operator - (const math::Quaternion<T, i>& lhs, const math::Quaternion<T, i>& rhs)
{
    math::Quaternion<T, i> tmp(lhs);
    return tmp -= rhs;
}

template<typename T, INSTRUCTION_SET i>
inline math::Quaternion<T, i> operator * (const math::Quaternion<T, i>& lhs, const math::Quaternion<T, i>& rhs)
{
    math::Quaternion<T, i> tmp(lhs);
    return tmp *= rhs;
}

template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 3, 1, FPU> operator * (const Matrix<T, 3, 1, FPU>& lhs, const Quaternion<T, is>& rhs)
{
    Matrix<T, 3, 1, is> tmp(rhs);
    return tmp *= lhs;
}

template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 3, 1, FPU> operator * (const Quaternion<T, is>& lhs, const Matrix<T, 3, 1, FPU>& rhs)
{
    Matrix<T, 3, 1, FPU> tmp(rhs);
    return tmp *= lhs;
}

template<typename T, INSTRUCTION_SET i>
inline math::Quaternion<T, i> operator * (const math::Quaternion<T, i>& lhs, T rhs)
{
    math::Quaternion<T, i> tmp(lhs);
    return tmp *= rhs;
}

template<typename T, INSTRUCTION_SET i>
inline math::Quaternion<T, i> operator / (const math::Quaternion<T, i>& lhs, T rhs)
{
    math::Quaternion<T, i> tmp(lhs);
    return tmp /= rhs;
}

/** print math::Quaternion */
template<typename T, INSTRUCTION_SET i>
inline std::ostream& operator << (std::ostream& os, const math::Quaternion<T, i>& quat)
{
    os << quat.x << ' ' << quat.y << ' ' << quat.z << ' ' << quat.w;
    return os;
}

/** scan math::Quaternion */
template<typename T, INSTRUCTION_SET i>
inline std::istream& operator >> (std::istream& is, math::Quaternion<T, i>& quat)
{
    is >> quat.x >> ' ' >> quat.y >> ' ' >> quat.z >> ' ' >> quat.w;
    return is;
}

} // namespace math
#ifdef SGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // GRAPHICS_MATH_QUATERNION_HPP
