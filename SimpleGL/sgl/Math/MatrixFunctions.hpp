#ifndef SIMPLE_GL_MATH_MATRIX_FUNCTIONS_H
#define SIMPLE_GL_MATH_MATRIX_FUNCTIONS_H

#include "Matrix.hpp"

#ifdef MSVC
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif

SGL_BEGIN_MATH_NAMESPACE

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
    return make_matrix( mat[0][0], mat[0][1], mat[0][2],
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

/** Get euler rotation angles from matrix: http://www.geometrictools.com/LibFoundation/Mathematics/Wm4Matrix3.inl.html
 * @param mat - rotation matrix
 * @param res[out] - output angles
 * @return true - if angles can be determined uniquely
 */
template <typename T>
inline bool to_euler_xyz(const Matrix<T, 3, 3>& mat, Matrix<T, 3, 1>& res)
{
    // +-           -+   +-                                        -+
    // | r00 r01 r02 |   |  cy*cz           -cy*sz            sy    |
    // | r10 r11 r12 | = |  cz*sx*sy+cx*sz   cx*cz-sx*sy*sz  -cy*sx |
    // | r20 r21 r22 |   | -cx*cz*sy+sx*sz   cz*sx+cx*sy*sz   cx*cy |
    // +-           -+   +-                                        -+
    T elem = mat(2);
    if (mat(2) < T(1))
    {
        if (mat(2) > T(-1))
        {
            // y_angle = asin(r02)
            // x_angle = atan2(-r12,r22)
            // z_angle = atan2(-r01,r00)
            res[1] = asin(mat(2));
            res[0] = atan2(-mat(5), mat(8));
            res[2] = atan2(-mat(1), mat(0));
            return true;
        }
        else
        {
            // y_angle = -pi/2
            // z_angle - x_angle = atan2(r10,r11)
            // WARNING.  The solution is not unique.  Choosing z_angle = 0.
            res[1] = -T(HALF_PI);
            res[0] = -atan2(mat(3), mat(4));
            res[2] = T(0);
            return false;
        }
    }
    else
    {
        // y_angle = +pi/2
        // z_angle + x_angle = atan2(r10,r11)
        // WARNING.  The solutions is not unique.  Choosing z_angle = 0.
        res[1] = T(HALF_PI);
        res[0] = atan2(mat(3), mat(4));
        res[2] = T(0);
        return false;
    }
}

/** Get euler rotation angles from matrix: http://www.geometrictools.com/LibFoundation/Mathematics/Wm4Matrix3.inl.html
 * @param mat - rotation matrix
 * @param res[out] - output angles
 * @return true - if angles can be determined uniquely
 */
template <typename T>
inline bool to_euler_xzy(const Matrix<T, 3, 3>& mat, Matrix<T, 3, 1>& res)
{
    // +-           -+   +-                                        -+
    // | r00 r01 r02 |   |  cy*cz           -sz      cz*sy          |
    // | r10 r11 r12 | = |  sx*sy+cx*cy*sz   cx*cz  -cy*sx+cx*sy*sz |
    // | r20 r21 r22 |   | -cx*sy+cy*sx*sz   cz*sx   cx*cy+sx*sy*sz |
    // +-           -+   +-                                        -+
    if (mat(1) < T(1))
    {
        if (mat(1) > -T(1))
        {
            // z_angle = asin(-r01)
            // x_angle = atan2(r21,r11)
            // y_angle = atan2(r02,r00)
            res.z = (T)asin(-(double)mat(1));
            res.x = atan2(mat(7), mat(4));
            res.y = atan2(mat(2), mat(0));
            return true;
        }
        else
        {
            // z_angle = +pi/2
            // y_angle - x_angle = atan2(-r20,r22)
            // WARNING.  The solution is not unique.  Choosing y_angle = 0.
            res.z = HALF_PI;
            res.x = -atan2(-mat(6) ,mat(8));
            res.y = 0;
            return false;
        }
    }
    else
    {
        // z_angle = -pi/2
        // y_angle + x_angle = atan2(-r20,r22)
        // WARNING.  The solution is not unique.  Choosing y_angle = 0.
        res.z = -HALF_PI;
        res.x = atan2(-mat(6), mat(8));
        res.y = 0;
        return false;
    }
}

/** Get 3x3 matrix determinant. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix4.inl */
template <typename T>
T get_determinant(const Matrix<T, 3, 3>& mat)
{
    T co00 = mat(4)*mat(8) - mat(5)*mat(7);
    T co10 = mat(5)*mat(6) - mat(3)*mat(8);
    T co20 = mat(3)*mat(7) - mat(4)*mat(6);

    return mat(0)*co00 + mat(1)*co10 + mat(2)*co20;
}

/** Get 4x4 matrix determinant. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix4.inl */
template<typename T>
T get_determinant(const Matrix<T, 4, 4>& mat)
{
    T a0 = mat( 0)*mat( 5) - mat( 1)*mat( 4);
    T a1 = mat( 0)*mat( 6) - mat( 2)*mat( 4);
    T a2 = mat( 0)*mat( 7) - mat( 3)*mat( 4);
    T a3 = mat( 1)*mat( 6) - mat( 2)*mat( 5);
    T a4 = mat( 1)*mat( 7) - mat( 3)*mat( 5);
    T a5 = mat( 2)*mat( 7) - mat( 3)*mat( 6);
    T b0 = mat( 8)*mat(13) - mat( 9)*mat(12);
    T b1 = mat( 8)*mat(14) - mat(10)*mat(12);
    T b2 = mat( 8)*mat(15) - mat(11)*mat(12);
    T b3 = mat( 9)*mat(14) - mat(10)*mat(13);
    T b4 = mat( 9)*mat(15) - mat(11)*mat(13);
    T b5 = mat(10)*mat(15) - mat(11)*mat(14);

    return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;;
}

/** Invert homogeneous matrix (faster than ordinary invert, but works only for homogeneous matrices) */
template<typename T>
inline Matrix<T, 4, 4> invert_homogeneous(const Matrix<T, 4, 4>& mat)
{
    Matrix<T, 4, 4> res;

	res[0][0] =   mat[0][0];
	res[0][1] =   mat[1][0];
	res[0][2] =   mat[2][0];
	res[0][3] = - mat[0][0] * mat[0][3] - mat[1][0] * mat[1][3] - mat[2][0] * mat[2][3];

	res[1][0] =   mat[0][1];
	res[1][1] =   mat[1][1];
	res[1][2] =   mat[2][1];
	res[1][3] = - mat[0][1] * mat[0][3] - mat[1][1] * mat[1][3] - mat[2][1] * mat[2][3];
	
	res[2][0] =   mat[0][2];
	res[2][1] =   mat[1][2];
	res[2][2] =   mat[2][2];
	res[2][3] = - mat[0][2] * mat[0][3] - mat[1][2] * mat[1][3] - mat[2][2] * mat[2][3];
	
	res[3][0] =   mat[3][0];
	res[3][1] =   mat[3][1];
	res[3][2] =   mat[3][2];
	res[3][3] =   mat[3][3];

	return res;
}

/** Invert 3x3 matrix using cofactors. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix3.inl */
template<typename T>
inline math::Matrix<T, 3, 3> invert(const math::Matrix<T, 3, 3>& mat)
{
    math::Matrix<T, 3, 3> inverse;

    // Compute the adjoint.
    inverse(0) = mat(4)*mat(8) - mat(5)*mat(7);
    inverse(1) = mat(2)*mat(7) - mat(1)*mat(8);
    inverse(2) = mat(1)*mat(5) - mat(2)*mat(4);
    inverse(3) = mat(5)*mat(6) - mat(3)*mat(8);
    inverse(4) = mat(0)*mat(8) - mat(2)*mat(6);
    inverse(5) = mat(2)*mat(3) - mat(0)*mat(5);
    inverse(6) = mat(3)*mat(7) - mat(4)*mat(6);
    inverse(7) = mat(1)*mat(6) - mat(0)*mat(7);
    inverse(8) = mat(0)*mat(4) - mat(1)*mat(3);

    T det = mat(0)*inverse(0) + mat(1)*inverse(3) + mat(2)*inverse(6);
    T invDet = T(1) / det;
    for (int i = 0; i<9; ++i) {
        inverse(i) *= invDet;
    }

    return inverse;
}

/** Invert 3x3 matrix if it can be inverted. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix3.inl */
template<typename T>
inline bool invert( const math::Matrix<T, 3, 3>& mat, math::Matrix<T, 3, 3>& inverse, T eps = std::numeric_limits<T>::min() )
{
    // Compute the adjoint.
    inverse(0) = mat(4)*mat(8) - mat(5)*mat(7);
    inverse(1) = mat(2)*mat(7) - mat(1)*mat(8);
    inverse(2) = mat(1)*mat(5) - mat(2)*mat(4);
    inverse(3) = mat(5)*mat(6) - mat(3)*mat(8);
    inverse(4) = mat(0)*mat(8) - mat(2)*mat(6);
    inverse(5) = mat(2)*mat(3) - mat(0)*mat(5);
    inverse(6) = mat(3)*mat(7) - mat(4)*mat(6);
    inverse(7) = mat(1)*mat(6) - mat(0)*mat(7);
    inverse(8) = mat(0)*mat(4) - mat(1)*mat(3);

    T det = mat(0)*inverse(0) + mat(1)*inverse(3) + mat(2)*inverse(6);
    if (abs(det) > eps)
    {
        T invDet = T(1) / det;
        for (int i = 0; i<9; ++i) {
            inverse(i) *= invDet;
        }

        return true;
    }

    return false;
}

/** Invert matrix using cofactors. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix4.inl */
template<typename T>
inline Matrix<T, 4, 4> invert(const Matrix<T, 4, 4>& mat)
{
    T a0 = mat( 0)*mat( 5) - mat( 1)*mat( 4);
    T a1 = mat( 0)*mat( 6) - mat( 2)*mat( 4);
    T a2 = mat( 0)*mat( 7) - mat( 3)*mat( 4);
    T a3 = mat( 1)*mat( 6) - mat( 2)*mat( 5);
    T a4 = mat( 1)*mat( 7) - mat( 3)*mat( 5);
    T a5 = mat( 2)*mat( 7) - mat( 3)*mat( 6);
    T b0 = mat( 8)*mat(13) - mat( 9)*mat(12);
    T b1 = mat( 8)*mat(14) - mat(10)*mat(12);
    T b2 = mat( 8)*mat(15) - mat(11)*mat(12);
    T b3 = mat( 9)*mat(14) - mat(10)*mat(13);
    T b4 = mat( 9)*mat(15) - mat(11)*mat(13);
    T b5 = mat(10)*mat(15) - mat(11)*mat(14);

    T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;

    Matrix<T, 4, 4> inverse;
    inverse( 0) = + mat( 5)*b5 - mat( 6)*b4 + mat( 7)*b3;
    inverse( 4) = - mat( 4)*b5 + mat( 6)*b2 - mat( 7)*b1;
    inverse( 8) = + mat( 4)*b4 - mat( 5)*b2 + mat( 7)*b0;
    inverse(12) = - mat( 4)*b3 + mat( 5)*b1 - mat( 6)*b0;
    inverse( 1) = - mat( 1)*b5 + mat( 2)*b4 - mat( 3)*b3;
    inverse( 5) = + mat( 0)*b5 - mat( 2)*b2 + mat( 3)*b1;
    inverse( 9) = - mat( 0)*b4 + mat( 1)*b2 - mat( 3)*b0;
    inverse(13) = + mat( 0)*b3 - mat( 1)*b1 + mat( 2)*b0;
    inverse( 2) = + mat(13)*a5 - mat(14)*a4 + mat(15)*a3;
    inverse( 6) = - mat(12)*a5 + mat(14)*a2 - mat(15)*a1;
    inverse(10) = + mat(12)*a4 - mat(13)*a2 + mat(15)*a0;
    inverse(14) = - mat(12)*a3 + mat(13)*a1 - mat(14)*a0;
    inverse( 3) = - mat( 9)*a5 + mat(10)*a4 - mat(11)*a3;
    inverse( 7) = + mat( 8)*a5 - mat(10)*a2 + mat(11)*a1;
    inverse(11) = - mat( 8)*a4 + mat( 9)*a2 - mat(11)*a0;
    inverse(15) = + mat( 8)*a3 - mat( 9)*a1 + mat(10)*a0;

    T invDet = T(1) / det;
    inverse( 0) *= invDet;
    inverse( 1) *= invDet;
    inverse( 2) *= invDet;
    inverse( 3) *= invDet;
    inverse( 4) *= invDet;
    inverse( 5) *= invDet;
    inverse( 6) *= invDet;
    inverse( 7) *= invDet;
    inverse( 8) *= invDet;
    inverse( 9) *= invDet;
    inverse(10) *= invDet;
    inverse(11) *= invDet;
    inverse(12) *= invDet;
    inverse(13) *= invDet;
    inverse(14) *= invDet;
    inverse(15) *= invDet;

    return inverse;
}

/** Invert matrix if it can be inverted. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix4.inl */
template<typename T>
inline bool invert( const Matrix<T, 4, 4>& mat, Matrix<T, 4, 4>& inverse, T eps = std::numeric_limits<T>::min() )
{
    T a0 = mat( 0)*mat( 5) - mat( 1)*mat( 4);
    T a1 = mat( 0)*mat( 6) - mat( 2)*mat( 4);
    T a2 = mat( 0)*mat( 7) - mat( 3)*mat( 4);
    T a3 = mat( 1)*mat( 6) - mat( 2)*mat( 5);
    T a4 = mat( 1)*mat( 7) - mat( 3)*mat( 5);
    T a5 = mat( 2)*mat( 7) - mat( 3)*mat( 6);
    T b0 = mat( 8)*mat(13) - mat( 9)*mat(12);
    T b1 = mat( 8)*mat(14) - mat(10)*mat(12);
    T b2 = mat( 8)*mat(15) - mat(11)*mat(12);
    T b3 = mat( 9)*mat(14) - mat(10)*mat(13);
    T b4 = mat( 9)*mat(15) - mat(11)*mat(13);
    T b5 = mat(10)*mat(15) - mat(11)*mat(14);

    T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    if ( abs(det) > eps)
    {
        inverse( 0) = + mat( 5)*b5 - mat( 6)*b4 + mat( 7)*b3;
        inverse( 4) = - mat( 4)*b5 + mat( 6)*b2 - mat( 7)*b1;
        inverse( 8) = + mat( 4)*b4 - mat( 5)*b2 + mat( 7)*b0;
        inverse(12) = - mat( 4)*b3 + mat( 5)*b1 - mat( 6)*b0;
        inverse( 1) = - mat( 1)*b5 + mat( 2)*b4 - mat( 3)*b3;
        inverse( 5) = + mat( 0)*b5 - mat( 2)*b2 + mat( 3)*b1;
        inverse( 9) = - mat( 0)*b4 + mat( 1)*b2 - mat( 3)*b0;
        inverse(13) = + mat( 0)*b3 - mat( 1)*b1 + mat( 2)*b0;
        inverse( 2) = + mat(13)*a5 - mat(14)*a4 + mat(15)*a3;
        inverse( 6) = - mat(12)*a5 + mat(14)*a2 - mat(15)*a1;
        inverse(10) = + mat(12)*a4 - mat(13)*a2 + mat(15)*a0;
        inverse(14) = - mat(12)*a3 + mat(13)*a1 - mat(14)*a0;
        inverse( 3) = - mat( 9)*a5 + mat(10)*a4 - mat(11)*a3;
        inverse( 7) = + mat( 8)*a5 - mat(10)*a2 + mat(11)*a1;
        inverse(11) = - mat( 8)*a4 + mat( 9)*a2 - mat(11)*a0;
        inverse(15) = + mat( 8)*a3 - mat( 9)*a1 + mat(10)*a0;

        T invDet = T(1) / det;
        inverse( 0) *= invDet;
        inverse( 1) *= invDet;
        inverse( 2) *= invDet;
        inverse( 3) *= invDet;
        inverse( 4) *= invDet;
        inverse( 5) *= invDet;
        inverse( 6) *= invDet;
        inverse( 7) *= invDet;
        inverse( 8) *= invDet;
        inverse( 9) *= invDet;
        inverse(10) *= invDet;
        inverse(11) *= invDet;
        inverse(12) *= invDet;
        inverse(13) *= invDet;
        inverse(14) *= invDet;
        inverse(15) *= invDet;

        return true;
    }

    return false;
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
inline Matrix<T, 4, 4> make_rotation_x(T angle)
{
    T c = cos(angle);
    T s = sin(angle);

    return make_matrix( T(1.0), T(0.0), T(0.0), T(0.0),
                        T(0.0), c,      -s,     T(0.0),
                        T(0.0), s,      c,      T(0.0),
                        T(0.0), T(0.0), T(0.0), T(1.0) );
}

template<typename T>
inline Matrix<T, 4, 4> make_rotation_y(T angle)
{
    T c = cos(angle);
    T s = sin(angle);

    return make_matrix( c,      T(0.0), s,      T(0.0),
                        T(0.0), T(1.0), T(0.0), T(0.0),
                        -s,     T(0.0), c,      T(0.0),
                        T(0.0), T(0.0), T(0.0), T(1.0) );
}

template<typename T>
inline Matrix<T, 4, 4> make_rotation_z(T angle)
{
    T c = cos(angle);
    T s = sin(angle);

    return make_matrix( c,      -s,     T(0.0), T(0.0),
                        s,      c,      T(0.0), T(0.0),
                        T(0.0), T(0.0), T(1.0), T(0.0),
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

SGL_END_MATH_NAMESPACE

#ifdef MSVC
#pragma warning( pop )
#endif

#endif // SIMPLE_GL_MATH_MATRIX_FUNCTIONS_H
