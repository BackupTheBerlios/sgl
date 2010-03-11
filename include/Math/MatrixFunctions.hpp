#ifndef SIMPLE_GL_MATH_MATRIX_FUNCTIONS_H
#define SIMPLE_GL_MATH_MATRIX_FUNCTIONS_H

#include "Matrix.hpp"

#ifdef MSVC
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

/** transpose matrix */
template<typename T, int n, int m>
inline Matrix<T, m, n> transpose(const Matrix<T, n, m>& mat)
{
    Matrix<T, m, n> res;
    for(int i = 0; i<n; ++i)
    {
        for(int j = 0; j<m; ++j) {
            res[i][j] = mat[j][i];
        }
    }

    return res;
}

/** transpose vector */
template<typename T, int n>
inline Matrix<T, 1, n> transpose(const Matrix<T, n, 1>& vec)
{
    Matrix<T, 1, n> res;
    std::copy(res.arr, res.arr + n, vec.arr);
    return res;
}

/** transpose vector */
template<typename T, int n>
inline Matrix<T, n, 1> transpose(const Matrix<T, 1, n>& vec)
{
    Matrix<T, n, 1> res;
    std::copy(res.arr, res.arr + n, vec.arr);
    return res;
}

/** Get translation component of the matrix. */
template<typename T>
inline Matrix<T, 3, 1> get_translation(const Matrix<T, 4, 4>& mat)
{
    return Matrix<T, 3, 1>( mat[0][3], mat[1][3], mat[2][3] );
}

/** Get translation component of the matrix. */
template<typename T>
inline Matrix<T, 3, 3> get_rotation(const Matrix<T, 4, 4>& mat)
{
    return make_mat( mat[0][0], mat[1][3], mat[2][3],
                     mat[1][0], mat[1][1], mat[1][2],
                     mat[2][0], mat[2][1], mat[2][2] );
}

/** Get scaling component of the matrix */
template<typename T>
inline Matrix<T, 3, 1> get_scaling(const Matrix<T, 3, 3>& mat)
{
    return Matrix<T, 3, 1>( sqrt(mat[0][0]*mat[0][0] + mat[1][0]*mat[1][0] + mat[2][0]*mat[2][0]),
                                sqrt(mat[0][1]*mat[0][1] + mat[1][1]*mat[1][1] + mat[2][1]*mat[2][1]),
                                sqrt(mat[0][2]*mat[0][2] + mat[1][2]*mat[1][2] + mat[2][2]*mat[2][2]) );
}

/** Get scaling component of the matrix */
template<typename T>
inline Matrix<T, 3, 1> get_scaling(const Matrix<T, 4, 4>& mat)
{
    return Matrix<T, 3, 1>( sqrt(mat[0][0]*mat[0][0] + mat[1][0]*mat[1][0] + mat[2][0]*mat[2][0]),
                                sqrt(mat[0][1]*mat[0][1] + mat[1][1]*mat[1][1] + mat[2][1]*mat[2][1]),
                                sqrt(mat[0][2]*mat[0][2] + mat[1][2]*mat[1][2] + mat[2][2]*mat[2][2]) );
}

/** Invert matrix. Code taken from Intel pdf "Streaming SIMD Extension - Inverse of 4x4 Matrix" */
template<typename T>
inline Matrix<T, 4, 4> invert(const Matrix<T, 4, 4>& mat)
{
    Matrix<T, 4, 3> tmp;
    Matrix<T, 4, 4> src;
    Matrix<T, 4, 4> res;
    T                   det;

    /* transpose matrix */
    for (int i = 0; i < 4; ++i)
    {
        src(i)      = mat(i*4);
        src(i + 4)  = mat(i*4 + 1);
        src(i + 8)  = mat(i*4 + 2);
        src(i + 12) = mat(i*4 + 3);
    }

    /* calculate pairs for first 8 elements (cofactors) */
    tmp(0)  = src(10) * src(15);
    tmp(1)  = src(11) * src(14);
    tmp(2)  = src(9)  * src(15);
    tmp(3)  = src(11) * src(13);
    tmp(4)  = src(9)  * src(14);
    tmp(5)  = src(10) * src(13);
    tmp(6)  = src(8)  * src(15);
    tmp(7)  = src(11) * src(12);
    tmp(8)  = src(8)  * src(14);
    tmp(9)  = src(10) * src(12);
    tmp(10) = src(8)  * src(13);
    tmp(11) = src(9)  * src(12);

    /* calculate first 8 elements (cofactors) */
    res(0)  = tmp(0)*src(5) + tmp(3)*src(6) + tmp(4)*src(7);
    res(0) -= tmp(1)*src(5) + tmp(2)*src(6) + tmp(5)*src(7);
    res(1)  = tmp(1)*src(4) + tmp(6)*src(6) + tmp(9)*src(7);
    res(1) -= tmp(0)*src(4) + tmp(7)*src(6) + tmp(8)*src(7);
    res(2)  = tmp(2)*src(4) + tmp(7)*src(5) + tmp(10)*src(7);
    res(2) -= tmp(3)*src(4) + tmp(6)*src(5) + tmp(11)*src(7);
    res(3)  = tmp(5)*src(4) + tmp(8)*src(5) + tmp(11)*src(6);
    res(3) -= tmp(4)*src(4) + tmp(9)*src(5) + tmp(10)*src(6);
    res(4)  = tmp(1)*src(1) + tmp(2)*src(2) + tmp(5)*src(3);
    res(4) -= tmp(0)*src(1) + tmp(3)*src(2) + tmp(4)*src(3);
    res(5)  = tmp(0)*src(0) + tmp(7)*src(2) + tmp(8)*src(3);
    res(5) -= tmp(1)*src(0) + tmp(6)*src(2) + tmp(9)*src(3);
    res(6)  = tmp(3)*src(0) + tmp(6)*src(1) + tmp(11)*src(3);
    res(6) -= tmp(2)*src(0) + tmp(7)*src(1) + tmp(10)*src(3);
    res(7)  = tmp(4)*src(0) + tmp(9)*src(1) + tmp(10)*src(2);
    res(7) -= tmp(5)*src(0) + tmp(8)*src(1) + tmp(11)*src(2);

    /* calculate pairs for second 8 elements (cofactors) */
    tmp(0)  = src(2)*src(7);
    tmp(1)  = src(3)*src(6);
    tmp(2)  = src(1)*src(7);
    tmp(3)  = src(3)*src(5);
    tmp(4)  = src(1)*src(6);
    tmp(5)  = src(2)*src(5);
    tmp(6)  = src(0)*src(7);
    tmp(7)  = src(3)*src(4);
    tmp(8)  = src(0)*src(6);
    tmp(9)  = src(2)*src(4);
    tmp(10) = src(0)*src(5);
    tmp(11) = src(1)*src(4);

    /* calculate second 8 elements (cofactors) */
    res(8)   = tmp(0)*src(13)  + tmp(3)*src(14)  + tmp(4)*src(15);
    res(8)  -= tmp(1)*src(13)  + tmp(2)*src(14)  + tmp(5)*src(15);
    res(9)   = tmp(1)*src(12)  + tmp(6)*src(14)  + tmp(9)*src(15);
    res(9)  -= tmp(0)*src(12)  + tmp(7)*src(14)  + tmp(8)*src(15);
    res(10)  = tmp(2)*src(12)  + tmp(7)*src(13)  + tmp(10)*src(15);
    res(10) -= tmp(3)*src(12)  + tmp(6)*src(13)  + tmp(11)*src(15);
    res(11)  = tmp(5)*src(12)  + tmp(8)*src(13)  + tmp(11)*src(14);
    res(11) -= tmp(4)*src(12)  + tmp(9)*src(13)  + tmp(10)*src(14);
    res(12)  = tmp(2)*src(10)  + tmp(5)*src(11)  + tmp(1)*src(9);
    res(12) -= tmp(4)*src(11)  + tmp(0)*src(9)   + tmp(3)*src(10);
    res(13)  = tmp(8)*src(11)  + tmp(0)*src(8)   + tmp(7)*src(10);
    res(13) -= tmp(6)*src(10)  + tmp(9)*src(11)  + tmp(1)*src(8);
    res(14)  = tmp(6)*src(9)   + tmp(11)*src(11) + tmp(3)*src(8);
    res(14) -= tmp(10)*src(11) + tmp(2)*src(8)   + tmp(7)*src(9);
    res(15)  = tmp(10)*src(10) + tmp(4)*src(8)   + tmp(9)*src(9);
    res(15) -= tmp(8)*src(9)   + tmp(11)*src(10) + tmp(5)*src(8);

    /* calculate determinant */
    det = src(0)*res(0)
        + src(1)*res(1)
        + src(2)*res(2)
        + src(3)*res(3);

    /* calculate matrix inverse */
    det = 1.0f / det;
    for ( int j = 0; j < 16; j++)
        res(j) *= det;

    return res;
}

template<typename T, int n>
inline Matrix<T, n, n> make_identity()
{
    Matrix<T, n, n> matrix;
    for (int i = 0; i<n; ++i)
    {
        for (int j = 0; j<n; ++j) {
            matrix[i][j] = T(i == j);
        }
    }

    return matrix;
}

template<typename T>
inline Matrix<T, 4, 4> make_translation(T x, T y, T z)
{
    return make_matrix( T(1.0), T(0.0), T(0.0), x,
                        T(0.0), T(1.0), T(0.0), y,
                        T(0.0), T(0.0), T(1.0), z,
                        T(0.0), T(0.0), T(0.0), T(1.0) );
}

template<typename T>
inline Matrix<T, 4, 4> make_scaling(T x, T y, T z)
{
    return make_matrix( T(x),   T(0.0), T(0.0), T(0.0),
                        T(0.0), T(y),   T(0.0), T(0.0),
                        T(0.0), T(0.0), T(z),   T(0.0),
                        T(0.0), T(0.0), T(0.0), T(1.0) );
}

template<typename T>
inline Matrix<T, 4, 4> make_rotation(T angle, const math::Matrix<T, 3, 1>& v)
{
    T c = cos(angle);
    T s = sin(angle);

    return make_matrix( v.x * v.x + (T(1) - v.x * v.x) * c, v.x * v.y * (T(1)- c) - v.z * s,    v.x * v.z * (T(1) - c) + v.y * s,   T(0),
                        v.x * v.y * (T(1) - c) + v.z * s,   v.y * v.y + (T(1) - v.y * v.y) * c, v.y * v.z * (T(1) - c) - v.x * s,   T(0),
                        v.x * v.z * (T(1) - c) - v.y * s,   v.y * v.z * (T(1) - c) + v.x * s,   v.z * v.z + (T(1) - v.z * v.z) * c, T(0),
                        T(0),                               T(0),                               T(0),                               T(1) );
}

template<typename T>
inline Matrix<T, 4, 4> make_ortho(T left, T right, T bottom, T top, T near_, T far_)
{
    T tx = (right + left) / (left - right);
    T ty = (top + bottom) / (bottom - top);
    T tz = (far_ + near_) / (near_ - far_);

    T x = T(2.0) / (right - left);
    T y = T(2.0) / (top - bottom);
    T z = T(2.0) / (near_ - far_);

    return make_matrix( x,      T(0.0), T(0.0), tx,
                        T(0.0), y,      T(0.0), ty,
                        T(0.0), T(0.0), z,      tz,
                        T(0.0), T(0.0), T(0.0), T(1.0) );
}

template<typename T>
inline Matrix<T, 4, 4> make_perspective(T fovy, T aspect, T zNear, T zFar)
{
    T f = T(1) / tan(fovy / 2);
	T d = zFar - zNear;

    return make_matrix( f / aspect, T(0),    T(0),     T(0),
                        T(0),       f,       T(0),     T(0),
                        T(0),       T(0),    zFar/d,   -zNear*zFar/d,
                        T(0),       T(0),    T(1),     T(0) );
}

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/** transpose matrix */
inline Matrix<float, 4, 4> transpose(const Matrix<float, 4, 4>& mat)
{
    Matrix<float, 4, 4> res(mat);
    _MM_TRANSPOSE4_PS(res[0].m128, res[1].m128, res[2].m128, res[3].m128);
    return res;
}

/** transpose vector */
inline Matrix<float, 1, 4> transpose(const Matrix<float, 4, 1>& vec)
{
    return Matrix<float, 1, 4>(vec.m128);
}

/** transpose vector */
inline Matrix<float, 4, 1> transpose(const Matrix<float, 1, 4>& vec)
{
    return Matrix<float, 4, 1>(vec.m128);
}

/** Invert matrix. Code taken from Intel pdf "Streaming SIMD Extension - Inverse of 4x4 Matrix" */
inline Matrix<float, 4, 4> invert(const Matrix<float, 4, 4>& mat)
{
    Matrix<float, 4, 4> res;

    float*       dst = &res[0].x;
    const float* src = &mat[0].x;

     __m128 minor0, minor1, minor2, minor3;
     __m128 row0, row1, row2, row3;
     __m128 det, tmp1;

     tmp1 = _mm_loadh_pi(_mm_loadl_pi(mat[0].m128, (__m64*)(src)), (__m64*)(src+ 4));
     row1 = _mm_loadh_pi(_mm_loadl_pi(mat[0].m128, (__m64*)(src+8)), (__m64*)(src+12));

     row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
     row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);

     tmp1 = _mm_loadh_pi(_mm_loadl_pi(mat[0].m128, (__m64*)(src+ 2)), (__m64*)(src+ 6));
     row3 = _mm_loadh_pi(_mm_loadl_pi(mat[0].m128, (__m64*)(src+10)), (__m64*)(src+14));

     row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
     row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);

     tmp1 = _mm_mul_ps(row2, row3);
     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

     minor0 = _mm_mul_ps(row1, tmp1);
     minor1 = _mm_mul_ps(row0, tmp1);

     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

     minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
     minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
     minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

     tmp1 = _mm_mul_ps(row1, row2);
     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

     minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
     minor3 = _mm_mul_ps(row0, tmp1);

     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

     minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
     minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
     minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);

     tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
     row2 = _mm_shuffle_ps(row2, row2, 0x4E);

     minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
     minor2 = _mm_mul_ps(row0, tmp1);

     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

     minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
     minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
     minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

     tmp1 = _mm_mul_ps(row0, row1);
     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

     minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
     minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);

     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

     minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
     minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

     tmp1 = _mm_mul_ps(row0, row3);
     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

     minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
     minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);

     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

     minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
     minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

     tmp1 = _mm_mul_ps(row0, row2);
     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

     minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
     minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));

     tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

     minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
     minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

     det = _mm_mul_ps(row0, minor0);
     det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
     det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
     tmp1 = _mm_rcp_ss(det);

     det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
     det = _mm_shuffle_ps(det, det, 0x00);

     // Store the result
     minor0 = _mm_mul_ps(det, minor0);
     _mm_storel_pi((__m64*)(dst), minor0);
     _mm_storeh_pi((__m64*)(dst+2), minor0);

     minor1 = _mm_mul_ps(det, minor1);
     _mm_storel_pi((__m64*)(dst+4), minor1);
     _mm_storeh_pi((__m64*)(dst+6), minor1);

     minor2 = _mm_mul_ps(det, minor2);
     _mm_storel_pi((__m64*)(dst+ 8), minor2);
     _mm_storeh_pi((__m64*)(dst+10), minor2);

     minor3 = _mm_mul_ps(det, minor3);
     _mm_storel_pi((__m64*)(dst+12), minor3);
     _mm_storeh_pi((__m64*)(dst+14), minor3);

     return res;
}

#endif


} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#ifdef MSVC
#pragma warning( pop )
#endif

#endif // SIMPLE_GL_MATH_MATRIX_FUNCTIONS_H
