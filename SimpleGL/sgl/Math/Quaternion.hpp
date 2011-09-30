#ifndef SIMPLE_GL_MATH_QUATERNION_HPP
#define SIMPLE_GL_MATH_QUATERNION_HPP

#include "Matrix.hpp"

SGL_BEGIN_MATH_NAMESPACE

template<typename T>
class Quaternion
{
public:
    Quaternion() {}

    Quaternion(const Quaternion& rhs) :
        x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

    explicit Quaternion(const Matrix<T, 3, 1>& vec, T _w = T(0)) :
        x(vec.x), y(vec.y), z(vec.z), w(_w) {}

    Quaternion(T _x, T _y, T _z, T _w) :
        x(_x), y(_y), z(_z), w(_w) {}

    explicit Quaternion(const Matrix<T, 4, 1>& vec) :
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
class Quaternion<float> :
    public sgl::Aligned<0x10>
{
public:
    typedef float               value_type;
    typedef Quaternion<float>   this_type;

public:
    Quaternion() {}

    Quaternion(const Quaternion& rhs) :
        x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

    explicit Quaternion(const Matrix<float, 3, 1>& vec, float _w = 0.0f) :
        x(vec.x), y(vec.y), z(vec.z), w(_w) {}

    Quaternion(float _x, float _y, float _z, float _w) :
        x(_x), y(_y), z(_z), w(_w) {}

    explicit Quaternion(const __m128& _m128) :
        m128(_m128)
    {}

    explicit Quaternion(const Matrix<float, 4, 1>& vec) :
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

template<typename T>
inline bool operator == (const Quaternion<T>& lhs, const Quaternion<T>& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w);
}

template<typename T>
inline bool operator != (const Quaternion<T>& lhs, const Quaternion<T>& rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y) || (lhs.z != rhs.z) || (lhs.w != rhs.w);
}

template<typename T>
inline Quaternion<T> operator - (const Quaternion<T>& rhs)
{
    return Quaternion<T>(-rhs.x, -rhs.y, -rhs.z, -rhs.w);
}

template<typename T>
inline Quaternion<T>& operator += (Quaternion<T>& lhs, const Quaternion<T>& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}

template<typename T>
inline Quaternion<T>& operator -= (Quaternion<T>& lhs, const Quaternion<T>& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}

template<typename T>
inline Quaternion<T>& operator *= (Quaternion<T>& lhs, const Quaternion<T>& rhs)
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

template<typename T>
inline Quaternion<T>& operator *= (Quaternion<T>& lhs, const math::Matrix<T, 3, 1>& rhs)
{
    T tmpX = lhs.x;
    T tmpY = lhs.y;
    T tmpZ = lhs.z;

    lhs.x = lhs.w * rhs.x + tmpY * rhs.z - tmpZ * rhs.y;
    lhs.y = lhs.w * rhs.y + tmpZ * rhs.y - tmpX * rhs.z;
    lhs.z = lhs.w * rhs.z + tmpX * rhs.y - tmpY * rhs.x;
    lhs.w =  tmpX * rhs.x - tmpY * rhs.y - tmpZ * rhs.z;

    return lhs;
}

template<typename T>
inline Quaternion<T>& operator *= (Quaternion<T>& lhs, T rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    lhs.w *= rhs;
    return lhs;
}

template<typename T>
inline Quaternion<T> operator * (Matrix<T, 3, 1>& lhs, const Quaternion<T>& rhs)
{
    Quaternion<T> res(lhs);
    return res *= rhs;
}

template<typename T>
inline Quaternion<T> operator * (const Quaternion<T>& lhs, Matrix<T, 3, 1>& rhs)
{
    Quaternion<T> res(lhs);
    return res *= rhs;
}

template<typename T>
inline Quaternion<T>& operator /= (Quaternion<T>& lhs, T rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    lhs.w /= rhs;
    return lhs;
}

// functions
template<typename T>
inline Matrix<T, 3, 1> rotate(const Quaternion<T>&   quat, 
                              const Matrix<T, 3, 1>& vec)
{
    // The actual formula is: rotate(q,v) = q*v*conj(q) = (2*q.w*q.w - 1)*v + 2*dot(q.v, v)*q.v + 2*q.w*cross(q.v, v)
    T w2    = T(2) * quat.w;
    T w21   = w2 * quat.w - T(1);
    T v2Dot = T(2) * (quat.x * vec.x + quat.y * vec.y + quat.z * vec.z);

    Matrix<T, 3, 1> res;
    {
        res.x = w21 * vec.x + v2Dot * quat.x + w2 * (quat.y * vec.z - vec.y * quat.z);
        res.y = w21 * vec.y + v2Dot * quat.y + w2 * (quat.z * vec.x - vec.z * quat.x);
        res.z = w21 * vec.z + v2Dot * quat.z + w2 * (quat.x * vec.y - vec.x * quat.y);
    }
    return res;
}

template<typename T>
inline Matrix<T, 4, 1> rotate(const Quaternion<T>&   quat, 
                              const Matrix<T, 4, 1>& vec)
{
    // The actual formula is: rotate(q,v) = q*v*conj(q) = (2*q.w*q.w - 1)*v + 2*dot(q.v, v)*q.v + 2*q.w*cross(q.v, v)
    T w2    = T(2) * quat.w;
    T w21   = w2 * quat.w - T(1);
    T v2Dot = T(2) * (quat.x * vec.x + quat.y * vec.y + quat.z * vec.z);

    Matrix<T, 4, 1> res;
    {
        res.x = w21 * vec.x + v2Dot * quat.x + w2 * (quat.y * vec.z - vec.y * quat.z);
        res.y = w21 * vec.y + v2Dot * quat.y + w2 * (quat.z * vec.x - vec.z * quat.x);
        res.z = w21 * vec.z + v2Dot * quat.z + w2 * (quat.x * vec.y - vec.x * quat.y);
        res.w = vec.w;
    }
    return res;
}

template<typename T>
inline Quaternion<T> conjugate(const Quaternion<T>& quat)
{
    return Quaternion<T>(-quat.x, -quat.y, -quat.z, quat.w);
}

template<typename T>
inline Quaternion<T> invert(const Quaternion<T>& quat)
{
    return conjugate(quat) / norm(quat);
}

template<typename T>
inline T norm(const Quaternion<T>& quat)
{
    return quat.x*quat.x + quat.y*quat.y + quat.z*quat.z + quat.w*quat.w;
}

template<typename T>
inline T magnitude(const Quaternion<T>& quat)
{
    return sqrt(quat.x*quat.x + quat.y*quat.y + quat.z*quat.z + quat.w*quat.w);
}

template<typename T>
inline T dot(const Quaternion<T>& a, const Quaternion<T>& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

/** Spherical interpolation.
 * @param a - start quaternion. Should be normalized.
 * @param b - end quaternion. Should be normalized.
 * @param value - interpolation value in [0, 1] segment
 */
template<typename T>
inline Quaternion<T> slerp(const Quaternion<T>& a, const Quaternion<T>& b, T value)
{
    T ca = dot(a, b);
    if ( abs(ca) - T(1.0) < T(1E-3) ) // cos(x) -> 1 => angle -> 0
    {
        // approximate: sin(x) = x
        return a * (T(1.0) - value) + b * value;
    }
    else
    {
        T angle = acos(ca);
        T sa    = sin(angle);
        T va    = value * angle;
        return ( a*std::sin(angle - va) + b*std::sin(va) ) / sa;
    }
}

/** normalize vector */
template<typename T>
inline Quaternion<T> normalize(const Quaternion<T>& quat)
{
    return quat / magnitude(quat);
}

template<typename T, int n>
inline Quaternion<T> from_matrix(const Matrix<T, n, n>& m)
{
    Quaternion<T> quat;

    T tr = m[0][0] + m[1][1] + m[2][2]; // trace of martix
    if (tr > 0.0)
    {
        // if trace positive than "w" is biggest component
        Quaternion<T> quat( m[1][2] - m[2][1],
                            m[2][0] - m[0][2],
                            m[0][1] - m[1][0],
                            tr + T(1.0) );
        return quat * T(0.5 / sqrt(quat.w));
    }
    else if ( (m[0][0] > m[1][1]) && (m[0][0] > m[2][2]) )
    {
        // Some of vector components is bigger
        Quaternion<T> quat( T(1.0) + m[0][0] - m[1][1] - m[2][2],
                            m[1][0] + m[0][1],
                            m[2][0] + m[0][2],
                            m[1][2] - m[2][1] );
        return quat * T(0.5 / sqrt(quat.x));

    }
    else if ( m[1][1] > m[2][2] )
    {
        Quaternion<T> quat( m[1][0] + m[0][1],
                            T(1.0) + m[1][1] - m[0][0] - m[2][2],
                            m[2][1] + m[1][2],
                            m[2][0] - m[0][2] );
        return quat * T(0.5 / sqrt(quat.y));

    }
    else
    {
        Quaternion<T> quat( m[2][0] + m[0][2],
                            m[2][1] + m[1][2],
                            T(1.0) + m[2][2] - m[0][0] - m[1][1],
                            m[0][1] - m[1][0] );
        return quat * T(0.5 / sqrt(quat.z));
    }
}

template<typename T>
inline Matrix<T, 3, 3> to_matrix_3x3(const Quaternion<T>& quat)
{
    T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = quat.x * T(2);
    y2 = quat.y * T(2);
    z2 = quat.z * T(2);

    xx = quat.x * x2;
    xy = quat.x * y2;
    xz = quat.x * z2;

    yy = quat.y * y2;
    yz = quat.y * z2;
    zz = quat.z * z2;

    wx = quat.w * x2;
    wy = quat.w * y2;
    wz = quat.w * z2;

    Matrix<T, 3, 3> matrix;
    {
        matrix[0][0] = T(1) - (yy + zz);
        matrix[1][0] = xy - wz;
        matrix[2][0] = xz + wy;

        matrix[0][1] = xy + wz;
        matrix[1][1] = T(1) - (xx + zz);
        matrix[2][1] = yz - wx;

        matrix[0][2] = xz - wy;
        matrix[1][2] = yz + wx;
        matrix[2][2] = T(1) - (xx + yy);
    }
    return matrix;
}

template<typename T>
inline Matrix<T, 4, 4> to_matrix_4x4(const Quaternion<T>&   quat, 
                                     const Matrix<T, 3, 1>& trans = Matrix<T, 3, 1>(T(0)))
{
    T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    x2 = quat.x * T(2);
    y2 = quat.y * T(2);
    z2 = quat.z * T(2);

    xx = quat.x * x2;
    xy = quat.x * y2;
    xz = quat.x * z2;

    yy = quat.y * y2;
    yz = quat.y * z2;
    zz = quat.z * z2;

    wx = quat.w * x2;
    wy = quat.w * y2;
    wz = quat.w * z2;

    Matrix<T, 4, 4> matrix;
    {
        matrix[0][0] = T(1) - (yy + zz);
        matrix[1][0] = xy - wz;
        matrix[2][0] = xz + wy;
        matrix[3][0] = T(0);

        matrix[0][1] = xy + wz;
        matrix[1][1] = T(1) - (xx + zz);
        matrix[2][1] = yz - wx;
        matrix[3][1] = T(0);

        matrix[0][2] = xz - wy;
        matrix[1][2] = yz + wx;
        matrix[2][2] = T(1) - (xx + yy);
        matrix[3][2] = T(0);

        matrix[0][3] = trans[0];
        matrix[1][3] = trans[1];
        matrix[2][3] = trans[2];
        matrix[3][3] = T(1);
    }
    return matrix;
}

template<typename T>
inline Quaternion<T> from_axis_angle(const Matrix<T, 3, 1>& axis)
{
    T angle = length(axis);
    return Quaternion<T>( axis * T(sin(angle * T(0.5))) / angle, cos(angle * T(0.5)) );
}

template<typename T>
inline Matrix<T, 3, 1> to_axis_angle(const Quaternion<T>& quat)
{
    Matrix<T, 3, 1> axis(quat.x, quat.y, quat.z);
    T vl  = length(axis);
    axis /= vl;
    if( quat.w < 0 ) {
        axis *= T( 2.0 * atan2(-vl, -quat.w) ); //-PI,0
    }
    else {
        axis *= T( 2.0 * atan2( vl,  quat.w) ); //0,PI
    }

    return axis;
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
inline Quaternion<float> operator - (const Quaternion<float>& rhs)
{
    __m128 zero    = _mm_set1_ps(0.0f);
    __m128 invQuat = _mm_sub_ps(zero, rhs.m128);
    return Quaternion<float>(invQuat);
}

template<>
inline Quaternion<float>& operator += (Quaternion<float>& lhs, const Quaternion<float>& rhs)
{
    lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
    return lhs;
}

template<>
inline Quaternion<float>& operator -= (Quaternion<float>& lhs, const Quaternion<float>& rhs)
{
    lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
    return lhs;
}
/*
template<>
inline Quaternion<float>& operator *= (Quaternion<float>& lhs, const Quaternion<float>& rhs)
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
inline Quaternion<float>& operator *= (Quaternion<float>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128      = _mm_mul_ps(lhs.m128, packed);
    return lhs;
}

template<>
inline Quaternion<float>& operator /= (Quaternion<float>& lhs, float rhs)
{
    __m128 packed = _mm_set1_ps(rhs);
    lhs.m128      = _mm_div_ps(lhs.m128, packed);
    return lhs;
}

inline Matrix<float, 4, 1> as_vec(const Quaternion<float>& quat)
{
    return Matrix<float, 4, 1>(quat.m128);
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
typedef Quaternion<float>      Quaternionf;
typedef Quaternion<double>     Quaterniond;

template<typename T>
inline math::Quaternion<T> operator + (const math::Quaternion<T>& lhs, const math::Quaternion<T>& rhs)
{
    math::Quaternion<T> tmp(lhs);
    return tmp += rhs;
}

template<typename T>
inline math::Quaternion<T> operator - (const math::Quaternion<T>& lhs, const math::Quaternion<T>& rhs)
{
    math::Quaternion<T> tmp(lhs);
    return tmp -= rhs;
}

template<typename T>
inline math::Quaternion<T> operator * (const math::Quaternion<T>& lhs, const math::Quaternion<T>& rhs)
{
    math::Quaternion<T> tmp(lhs);
    return tmp *= rhs;
}

template<typename T>
inline Matrix<T, 3, 1> operator * (const Matrix<T, 3, 1>& lhs, const Quaternion<T>& rhs)
{
    Matrix<T, 3, 1> tmp(lhs);
    return tmp *= rhs;
}

template<typename T>
inline Matrix<T, 3, 1> operator * (const Quaternion<T>& lhs, const Matrix<T, 3, 1>& rhs)
{
    Matrix<T, 3, 1> tmp(rhs);
    return tmp *= lhs;
}

template<typename T>
inline math::Quaternion<T> operator * (const math::Quaternion<T>& lhs, T rhs)
{
    math::Quaternion<T> tmp(lhs);
    return tmp *= rhs;
}

template<typename T>
inline math::Quaternion<T> operator / (const math::Quaternion<T>& lhs, T rhs)
{
    math::Quaternion<T> tmp(lhs);
    return tmp /= rhs;
}

/** print math::Quaternion */
template<typename T>
inline std::ostream& operator << (std::ostream& os, const math::Quaternion<T>& quat)
{
    os << quat.x << ' ' << quat.y << ' ' << quat.z << ' ' << quat.w;
    return os;
}

/** scan math::Quaternion */
template<typename T>
inline std::istream& operator >> (std::istream& is, math::Quaternion<T>& quat)
{
    is >> quat.x >> ' ' >> quat.y >> ' ' >> quat.z >> ' ' >> quat.w;
    return is;
}

SGL_END_MATH_NAMESPACE

#endif // SIMPLE_GL_MATH_QUATERNION_HPP
