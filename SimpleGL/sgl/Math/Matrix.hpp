/**
 * \author DikobrAz
 * File contains matrix definition and common matrix operations. If SIMPLE_GL_USE_SSE is ON, 
 * then 4x4 float matrices will be optimized using sse by default.
 * Don't forget to use aligned containers for sse Matrix4f defined in 'Containers.hpp'.
 * @see Vector.hpp
 * @see Containers.hpp
 */

#ifndef SIMPLE_GL_MATH_MATRIX_HPP
#define SIMPLE_GL_MATH_MATRIX_HPP

#include "Vector.hpp"

#ifdef MSVC
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif

SGL_BEGIN_MATH_NAMESPACE

template<typename T>
class Matrix<T, 1, 1> {};

/** Matrix class. */
template<typename ValueType,
         int n,
         int m>
class Matrix
{
public:
    typedef Matrix<ValueType, n, m> this_type;
    typedef Matrix<ValueType, 1, m> row_type;

    static const int num_rows     = n;
    static const int num_columns  = m;
    static const int num_elements = n*m;

public:
    Matrix() {}
    Matrix(const this_type& matrix) { std::copy(matrix.rows, matrix.rows + n, rows); }
	
    explicit Matrix(ValueType val)
	{
        std::fill(rows, rows + n, row_type(val));
	}

    template<typename T>
    explicit Matrix(const Matrix<T, n, m>& matrix)
    {
		for (int i = 0; i<n; ++i) {
			rows[i] = row_type(matrix[i]);
		}
    }

    row_type& operator [] (unsigned int i)
    {
        assert(i < n);
        return rows[i];
    }

    const row_type& operator [] (unsigned int i) const
    {
        assert(i < n);
        return rows[i];
    }

    /** Get plain matrix data */
    ValueType* data() { return rows[0].arr; }

    /** Get plain matrix data */
    const ValueType* data() const { return rows[0].arr; }

    /** Get item using linear index */
    ValueType& operator () (unsigned int i)
    {
        assert(i < n * m);
        return *( data() + i );
    }

    /** Get item using linear index */
    ValueType operator () (unsigned int i) const
    {
        assert(i < n * m);
        return *( data() + i );
    }

protected:
    row_type rows[n];
};


/** R3 matrix class. */
template<typename T>
class Matrix<T, 3, 3>
{
public:
    typedef MatrixBase<T, 3, 3>     base_type;
    typedef Matrix<T, 3, 3>         this_type;
    typedef Matrix<T, 1, 3>         row_type;

    static const int num_rows     = 3;
    static const int num_columns  = 3;
    static const int num_elements = 9;

public:
    Matrix() {}
    Matrix(const this_type& matrix)
    {
        rows[0] = matrix.rows[0];
        rows[1] = matrix.rows[1];
        rows[2] = matrix.rows[2];
    }

	Matrix(T _00, T _01, T _02,
           T _10, T _11, T _12,
           T _20, T _21, T _22)
	{
        rows[0] = row_type(_00, _01, _02);
        rows[1] = row_type(_10, _11, _12);
        rows[2] = row_type(_20, _21, _22);
	}

	explicit Matrix(T val)
	{
        rows[0] = row_type(val);
        rows[1] = rows[0];
        rows[2] = rows[0];
	}

    template<typename Y>
    explicit Matrix(const Matrix<Y, 3, 3>& matrix)
    {
        rows[0] = row_type(matrix.rows[0]);
        rows[1] = row_type(matrix.rows[1]);
        rows[2] = row_type(matrix.rows[2]);
    }

    row_type& operator [] (unsigned int i)
    {
        assert(i < 3);
        return rows[i];
    }

    const row_type& operator [] (unsigned int i) const
    {
        assert(i < 3);
        return rows[i];
    }

    /** Get plain matrix data */
    T* data() { return rows[0].arr; }

    /** Get plain matrix data */
    const T* data() const { return rows[0].arr; }

    /** Get item using linear index */
    T& operator () (unsigned int i)
    {
        assert(i < 9);
        return *( data() + i );
    }

    /** Get item using linear index */
    T operator () (unsigned int i) const
    {
        assert(i < 9);
        return *( data() + i );
    }

protected:
    row_type rows[3];
};

/** Homogeneous matrix class. */
template<typename T>
class Matrix<T, 4, 4>
{
public:
    typedef MatrixBase<T, 4, 4>     base_type;
    typedef Matrix<T, 4, 4>         this_type;
    typedef Matrix<T, 1, 4>         row_type;

    static const int num_rows     = 4;
    static const int num_columns  = 4;
    static const int num_elements = 16;

public:
    Matrix() {}
    Matrix(const this_type& matrix)
    {
        rows[0] = matrix.rows[0];
        rows[1] = matrix.rows[1];
        rows[2] = matrix.rows[2];
        rows[3] = matrix.rows[3];
    }

	Matrix(T _00, T _01, T _02, T _03,
           T _10, T _11, T _12, T _13,
           T _20, T _21, T _22, T _23,
           T _30, T _31, T _32, T _33)
	{
        rows[0] = row_type(_00, _01, _02, _03);
        rows[1] = row_type(_10, _11, _12, _13);
        rows[2] = row_type(_20, _21, _22, _23);
        rows[3] = row_type(_30, _31, _32, _33);
	}

	explicit Matrix(T val)
	{
        rows[0] = row_type(val);
        rows[1] = rows[0];
        rows[2] = rows[0];
        rows[3] = rows[0];
	}

    template<typename Y>
    explicit Matrix(const Matrix<Y, 4, 4>& matrix)
    {
        rows[0] = row_type(matrix.rows[0]);
        rows[1] = row_type(matrix.rows[1]);
        rows[2] = row_type(matrix.rows[2]);
        rows[3] = row_type(matrix.rows[3]);
    }

    row_type& operator [] (unsigned int i)
    {
        assert(i < 4);
        return rows[i];
    }

    const row_type& operator [] (unsigned int i) const
    {
        assert(i < 4);
        return rows[i];
    }

    /** Get plain matrix data */
    T* data() { return rows[0].arr; }

    /** Get plain matrix data */
    const T* data() const { return rows[0].arr; }

    /** Get item using linear index */
    T& operator () (unsigned int i)
    {
        assert(i < 16);
        return *( data() + i );
    }

    /** Get item using linear index */
    T operator () (unsigned int i) const
    {
        assert(i < 16);
        return *( data() + i );
    }

    /** Get identity matrix */
    static this_type identity()
    {
        return this_type(T(1), T(0), T(0), T(0),
                         T(0), T(1), T(0), T(0),
                         T(0), T(0), T(1), T(0),
                         T(0), T(0), T(0), T(1));
    }

    /** Make this matrix identity */
    void make_identity()
    {
        rows[0] = row_type(T(1), 0,    0,    0);
        rows[1] = row_type(0,    T(1), 0,    0);
        rows[2] = row_type(0,    0,    T(1), 0);
        rows[3] = row_type(0,    0,    0,    T(1));
    }

    /** Get translation transformation matrix */
    static this_type translation(T x, T y, T z)
    {
        return this_type( T(1), T(0), T(0), x,
                          T(0), T(1), T(0), y,
                          T(0), T(0), T(1), z,
                          T(0), T(0), T(0), T(1) );
    }

    /** Get scaling transformation matrix */
    static this_type scaling(T x, T y, T z)
    {
        return this_type( x,    T(0), T(0), T(0),
                          T(0), y,    T(0), T(0),
                          T(0), T(0), z,    T(0),
                          T(0), T(0), T(0), T(1) );
    }

    /** Get rotation around x transformation matrix */
    static this_type rotation_x(T angle)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( T(1), T(0), T(0), T(0),
                          T(0), c,    -s,   T(0),
                          T(0), s,    c,    T(0),
                          T(0), T(0), T(0), T(1) );
    }

    /** Get rotation around y transformation matrix */
    static this_type rotation_y(T angle)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( c,    T(0), s,    T(0),
                          T(0), T(1), T(0), T(0),
                          -s,   T(0), c,    T(0),
                          T(0), T(0), T(0), T(1) );
    }

    /** Get rotation around z transformation matrix */
    static this_type rotation_z(T angle)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( c,    -s,   T(0), T(0),
                          s,    c,    T(0), T(0),
                          T(0), T(0), T(1), T(0),
                          T(0), T(0), T(0), T(1) );
    }

    /** Get rotation around arbitrary axis transformation matrix */
    static this_type rotation(T angle, const Matrix<T, 3, 1>& v)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( v.x * v.x + (T(1) - v.x * v.x) * c, v.x * v.y * (T(1)- c) - v.z * s,    v.x * v.z * (T(1) - c) + v.y * s,   T(0),
                          v.x * v.y * (T(1) - c) + v.z * s,   v.y * v.y + (T(1) - v.y * v.y) * c, v.y * v.z * (T(1) - c) - v.x * s,   T(0),
                          v.x * v.z * (T(1) - c) - v.y * s,   v.y * v.z * (T(1) - c) + v.x * s,   v.z * v.z + (T(1) - v.z * v.z) * c, T(0),
                          T(0),                               T(0),                               T(0),                               T(1) );
    }

    /** Get ortho projection matrix */
    static this_type ortho(T left,   T right,
                           T bottom, T top,
                           T near_,  T far_)
    {
        T tx = (right + left) / (left - right);
        T ty = (top + bottom) / (bottom - top);
        T tz = (far_ + near_) / (near_ - far_);

        T x = T(2) / (right - left);
        T y = T(2) / (top - bottom);
        T z = T(2) / (near_ - far_);

        return this_type( x,    T(0), T(0), tx,
                          T(0), y,    T(0), ty,
                          T(0), T(0), z,    tz,
                          T(0), T(0), T(0), T(1) );
    }

    /** Get perspective projection matrix */
    static this_type perspective(T fovy, T aspect, T zNear, T zFar)
    {
        T f = T(1) / tan(fovy / T(2));
	    T d = zFar - zNear;
        return this_type( f / aspect, T(0),    T(0),     T(0),
                          T(0),       f,       T(0),     T(0),
                          T(0),       T(0),    zFar/d,   -zNear*zFar/d,
                          T(0),       T(0),    T(1),     T(0) );
    }

protected:
    row_type rows[4];
};

#ifdef SIMPLE_GL_USE_SSE

/** Homogeneous matrix class. */
template<>
class Matrix<float, 4, 4> :
    public sgl::Aligned<0x40> // If the aligment is 0x10 then sizeof(Matrix<float, 4, 4>) == 80
{
public:
    typedef Matrix<float, 4, 4> this_type;
    typedef Matrix<float, 1, 4> row_type;

    static const int num_rows     = 4;
    static const int num_columns  = 4;
    static const int num_elements = 16;

public:
    Matrix() {}
    Matrix(const this_type& matrix)
    {
        rows[0] = matrix.rows[0];
        rows[1] = matrix.rows[1];
        rows[2] = matrix.rows[2];
        rows[3] = matrix.rows[3];
    }

	Matrix(float _00, float _01, float _02, float _03,
           float _10, float _11, float _12, float _13,
           float _20, float _21, float _22, float _23,
           float _30, float _31, float _32, float _33)
	{
        rows[0] = row_type(_00, _01, _02, _03);
        rows[1] = row_type(_10, _11, _12, _13);
        rows[2] = row_type(_20, _21, _22, _23);
        rows[3] = row_type(_30, _31, _32, _33);
	}

	explicit Matrix(float val)
	{
        rows[0].m128 = _mm_set1_ps(val);
        rows[1].m128 = _mm_set1_ps(val);
        rows[2].m128 = _mm_set1_ps(val);
        rows[3].m128 = _mm_set1_ps(val);
	}

    template<typename Y>
    explicit Matrix(const Matrix<Y, 4, 4>& matrix)
    {
        rows[0] = row_type(matrix.rows[0]);
        rows[1] = row_type(matrix.rows[1]);
        rows[2] = row_type(matrix.rows[2]);
        rows[3] = row_type(matrix.rows[3]);
    }

    row_type& operator [] (unsigned int i)
    {
        assert(i < 4);
        return rows[i];
    }

    const row_type& operator [] (unsigned int i) const
    {
        assert(i < 4);
        return rows[i];
    }

    /** Get plain matrix data */
    float* data() { return rows[0].arr; }

    /** Get plain matrix data */
    const float* data() const { return rows[0].arr; }

    /** Get item using linear index */
    float& operator () (unsigned int i)
    {
        assert(i < 16);
        return *( data() + i );
    }

    /** Get item using linear index */
    float operator () (unsigned int i) const
    {
        assert(i < 16);
        return *( data() + i );
    }

    /** Get identity matrix */
    static Matrix<float, 4, 4> identity()
    {
        return this_type(1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);
    }

    /** Make this matrix identity */
    void make_identity()
    {
        rows[0].m128 = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
        rows[1].m128 = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
        rows[2].m128 = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
        rows[3].m128 = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
    }

    /** Get translation transformation matrix */
    static this_type translation(float x, float y, float z)
    {
        return this_type( 1.0f, 0.0f, 0.0f, x,
                          0.0f, 1.0f, 0.0f, y,
                          0.0f, 0.0f, 1.0f, z,
                          0.0f, 0.0f, 0.0f, 1.0f );
    }

    /** Get scaling transformation matrix */
    static this_type scaling(float x, float y, float z)
    {
        return this_type( x,    0.0f, 0.0f, 0.0f,
                          0.0f, y,    0.0f, 0.0f,
                          0.0f, 0.0f, z,    0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f );
    }

    /** Get rotation around x transformation matrix */
    static this_type rotation_x(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);
        return this_type( 1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, c,    -s,   0.0f,
                          0.0f, s,    c,    0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f );
    }

    /** Get rotation around y transformation matrix */
    static this_type rotation_y(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);
        return this_type( c,    0.0f, s,    0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          -s,   0.0f, c,    0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f );
    }

    /** Get rotation around z transformation matrix */
    static this_type rotation_z(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);
        return this_type( c,    -s,   0.0f, 0.0f,
                          s,    c,    0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f );
    }

    /** Get rotation around arbitrary axis transformation matrix */
    static this_type rotation(float angle, const Matrix<float, 3, 1>& v)
    {
        float c = cos(angle);
        float s = sin(angle);
        return this_type( v.x * v.x + (1.0f - v.x * v.x) * c, v.x * v.y * (1.0f- c) - v.z * s,    v.x * v.z * (1.0f - c) + v.y * s,   0.0f,
                          v.x * v.y * (1.0f - c) + v.z * s,   v.y * v.y + (1.0f - v.y * v.y) * c, v.y * v.z * (1.0f - c) - v.x * s,   0.0f,
                          v.x * v.z * (1.0f - c) - v.y * s,   v.y * v.z * (1.0f - c) + v.x * s,   v.z * v.z + (1.0f - v.z * v.z) * c, 0.0f,
                          0.0f,                               0.0f,                               0.0f,                               1.0f );
    }

    /** Get ortho projection matrix */
    static this_type ortho(float left,   float right,
                           float bottom, float top,
                           float near_,  float far_)
    {
        float tx = (right + left) / (left - right);
        float ty = (top + bottom) / (bottom - top);
        float tz = (far_ + near_) / (near_ - far_);

        float x = 2.0f / (right - left);
        float y = 2.0f / (top - bottom);
        float z = 2.0f / (near_ - far_);

        return this_type( x,    0.0f, 0.0f, tx,
                          0.0f, y,    0.0f, ty,
                          0.0f, 0.0f, z,    tz,
                          0.0f, 0.0f, 0.0f, 1.0f );
    }

    /** Get perspective projection matrix */
    static this_type perspective(float fovy, float aspect, float zNear, float zFar)
    {
        float f = 1.0f / tan(fovy * 0.5f);
	    float d = zFar - zNear;
        return this_type( f / aspect, 0.0f,    0.0f,     0.0f,
                          0.0f,       f,       0.0f,     0.0f,
                          0.0f,       0.0f,    zFar/d,   -zNear*zFar/d,
                          0.0f,       0.0f,    1.0f,     0.0f );
    }

protected:
    row_type rows[4];
};

#endif // SIMPLE_GL_USE_SSE

// default matrices
typedef Matrix<double, 4, 4>           Matrix4x4d; 
typedef Matrix<double, 4, 3>           Matrix4x3d;
typedef Matrix<double, 4, 2>           Matrix4x2d;
typedef Matrix<double, 3, 4>           Matrix3x4d;
typedef Matrix<double, 3, 3>           Matrix3x3d;
typedef Matrix<double, 3, 2>           Matrix3x2d;
typedef Matrix<double, 2, 4>           Matrix2x4d;
typedef Matrix<double, 2, 3>           Matrix2x3d;
typedef Matrix<double, 2, 2>           Matrix2x2d;
typedef Matrix<double, 4, 4>           Matrix4d;   
typedef Matrix<double, 3, 3>           Matrix3d;
typedef Matrix<double, 2, 2>           Matrix2d;

typedef Matrix<float, 4, 4>            Matrix4x4f;
typedef Matrix<float, 4, 3>            Matrix4x3f;
typedef Matrix<float, 4, 2>            Matrix4x2f;
typedef Matrix<float, 3, 4>            Matrix3x4f;
typedef Matrix<float, 3, 3>            Matrix3x3f;
typedef Matrix<float, 3, 2>            Matrix3x2f;
typedef Matrix<float, 2, 4>            Matrix2x4f;
typedef Matrix<float, 2, 3>            Matrix2x3f;
typedef Matrix<float, 2, 2>            Matrix2x2f;
typedef Matrix<float, 4, 4>            Matrix4f;
typedef Matrix<float, 3, 3>            Matrix3f;
typedef Matrix<float, 2, 2>            Matrix2f;

typedef Matrix<int, 4, 4>              Matrix4x4i; 
typedef Matrix<int, 4, 3>              Matrix4x3i;
typedef Matrix<int, 4, 2>              Matrix4x2i;
typedef Matrix<int, 3, 4>              Matrix3x4i;
typedef Matrix<int, 3, 3>              Matrix3x3i;
typedef Matrix<int, 3, 2>              Matrix3x2i;
typedef Matrix<int, 2, 4>              Matrix2x4i;
typedef Matrix<int, 2, 3>              Matrix2x3i;
typedef Matrix<int, 2, 2>              Matrix2x2i;
typedef Matrix<int, 4, 4>              Matrix4i;   
typedef Matrix<int, 3, 3>              Matrix3i;
typedef Matrix<int, 2, 2>              Matrix2i;

typedef Matrix<unsigned, 4, 4>         Matrix4x4ui; 
typedef Matrix<unsigned, 4, 3>         Matrix4x3ui;
typedef Matrix<unsigned, 4, 2>         Matrix4x2ui;
typedef Matrix<unsigned, 3, 4>         Matrix3x4ui;
typedef Matrix<unsigned, 3, 3>         Matrix3x3ui;
typedef Matrix<unsigned, 3, 2>         Matrix3x2ui;
typedef Matrix<unsigned, 2, 4>         Matrix2x4ui;
typedef Matrix<unsigned, 2, 3>         Matrix2x3ui;
typedef Matrix<unsigned, 2, 2>         Matrix2x2ui;
typedef Matrix<unsigned, 4, 4>         Matrix4ui;   
typedef Matrix<unsigned, 3, 3>         Matrix3ui;
typedef Matrix<unsigned, 2, 2>         Matrix2ui;

typedef Matrix<bool, 4, 4>             Matrix4x4b; 
typedef Matrix<bool, 4, 3>             Matrix4x3b;
typedef Matrix<bool, 4, 2>             Matrix4x2b;
typedef Matrix<bool, 3, 4>             Matrix3x4b;
typedef Matrix<bool, 3, 3>             Matrix3x3b;
typedef Matrix<bool, 3, 2>             Matrix3x2b;
typedef Matrix<bool, 2, 4>             Matrix2x4b;
typedef Matrix<bool, 2, 3>             Matrix2x3b;
typedef Matrix<bool, 2, 2>             Matrix2x2b;
typedef Matrix<bool, 4, 4>             Matrix4b;   
typedef Matrix<bool, 3, 3>             Matrix3b;
typedef Matrix<bool, 2, 2>             Matrix2b;

/** add per component */
template<typename T, int n, int m>
inline Matrix<T, n, m>& operator += (Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] += rhs[i];
    return lhs;
}

/** subtract per component */
template<typename T, int n, int m>
inline Matrix<T, n, m>& operator -= (Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] -= rhs[i];
    return lhs;
}

/** mul matrices */
template<typename T, int n>
inline Matrix<T, n, n>& operator *= (Matrix<T, n, n>& lhs, const Matrix<T, n, n>& rhs)
{
    Matrix<T, n, n> res;
    for (int i = 0; i<n; ++i)
    {
        for (int j = 0; j<n; ++j)
        {
            res[i][j] = lhs[i][0] * rhs[0][j];
            for (int k = 1; k<n; ++k)
                res[i][j] += lhs[i][k] * rhs[k][j];
        }
    }

    return lhs = res;
}

/** mul per component */
template<typename T, int n>
inline Matrix<T, n, n>& operator *= (Matrix<T, n, n>& lhs, T rhs)
{
    for (int i = 0; i<n; ++i) {
        lhs[i] *= rhs;
    }

    return lhs;
}

/** div per component */
template<typename T, typename Y, int n, int m>
inline Matrix<T, n, m>& operator /= (Matrix<T, n, m>& lhs, Y rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] /= static_cast<T>(rhs);
    return lhs;
}

/** compare matrices */
template<typename T, int n, int m>
inline bool operator == (const Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
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
template<typename T, int n, int m>
inline bool operator != (const Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
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
inline Matrix<float, 4, 4>& operator += (Matrix<float, 4, 4>& lhs, const Matrix<float, 4, 4>& rhs)
{
    lhs[0].m128 = _mm_add_ps(lhs[0].m128, rhs[0].m128);
    lhs[1].m128 = _mm_add_ps(lhs[1].m128, rhs[1].m128);
    lhs[2].m128 = _mm_add_ps(lhs[2].m128, rhs[2].m128);
    lhs[3].m128 = _mm_add_ps(lhs[3].m128, rhs[3].m128);
    return lhs;
}

/** subtract per component */
inline Matrix<float, 4, 4>& operator -= (Matrix<float, 4, 4>& lhs, const Matrix<float, 4, 4>& rhs)
{
    lhs[0].m128 = _mm_sub_ps(lhs[0].m128, rhs[0].m128);
    lhs[1].m128 = _mm_sub_ps(lhs[1].m128, rhs[1].m128);
    lhs[2].m128 = _mm_sub_ps(lhs[2].m128, rhs[2].m128);
    lhs[3].m128 = _mm_sub_ps(lhs[3].m128, rhs[3].m128);
    return lhs;
}

/** mul matrices */
inline Matrix<float, 4, 4>& operator *= (Matrix<float, 4, 4>& lhs, const Matrix<float, 4, 4>& rhs)
{
#ifdef SIMPLE_GL_USE_SSE4

    // load transposed matrix
    __m128 matrix0 = _mm_load_ps(&rhs[0].x);
    __m128 matrix1 = _mm_load_ps(&rhs[1].x);
    __m128 matrix2 = _mm_load_ps(&rhs[2].x);
    __m128 matrix3 = _mm_load_ps(&rhs[3].x);
    _MM_TRANSPOSE4_PS(matrix0, matrix1, matrix2, matrix3);

    __m128 row, dotProd;
    #define __CALC_ROW(i)\
        row         = lhs[i].m128;\
        lhs[i].m128 = _mm_dp_ps(row, matrix0, 0xEE);\
        dotProd     = _mm_dp_ps(row, matrix1, 0xEE);\
        lhs[i].m128 = _mm_blend_ps( lhs[i].m128, dotProd, _MM_SHUFFLE(0, 1, 1, 1) );\
        dotProd     = _mm_dp_ps(row, matrix2, 0xEE);\
        lhs[i].m128 = _mm_blend_ps( lhs[i].m128, dotProd, _MM_SHUFFLE(0, 0, 1, 1) );\
        dotProd     = _mm_dp_ps(row, matrix3, 0xEE);\
        lhs[i].m128 = _mm_blend_ps( lhs[i].m128, dotProd, _MM_SHUFFLE(0, 0, 0, 1) );

    // calculate
    __CALC_ROW(0)
    __CALC_ROW(1)
    __CALC_ROW(2)
    __CALC_ROW(3)
    #undef __CALC_ROW

    return lhs;
#else // SSE2
    __m128 row0;
    __m128 row1;
    __m128 row2;
    __m128 row3;
    #define __CALC_ROW(i)\
        row0 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(0, 0, 0, 0) );\
        row1 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(1, 1, 1, 1) );\
        row2 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(2, 2, 2, 2) );\
        row3 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(3, 3, 3, 3) );\
        \
        row0 = _mm_mul_ps(row0, rhs[0].m128);\
        row1 = _mm_mul_ps(row1, rhs[1].m128);\
        row2 = _mm_mul_ps(row2, rhs[2].m128);\
        row3 = _mm_mul_ps(row3, rhs[3].m128);\
        \
        lhs[i].m128 = _mm_add_ps(row0, row1);\
        lhs[i].m128 = _mm_add_ps(lhs[i].m128, row2);\
        lhs[i].m128 = _mm_add_ps(lhs[i].m128, row3);

    // calculate
    __CALC_ROW(0)
    __CALC_ROW(1)
    __CALC_ROW(2)
    __CALC_ROW(3)
    #undef __CALC_ROW

    return lhs;
#endif
}

/** mul per component */
inline Matrix<float, 4, 4>& operator *= (Matrix<float, 4, 4>& lhs, float rhs)
{
    __m128 valPacked = _mm_set1_ps(rhs);
    lhs[0].m128      = _mm_mul_ps(lhs[0].m128, valPacked);
    lhs[1].m128      = _mm_mul_ps(lhs[1].m128, valPacked);
    lhs[2].m128      = _mm_mul_ps(lhs[2].m128, valPacked);
    lhs[3].m128      = _mm_mul_ps(lhs[3].m128, valPacked);

    return lhs;
}

/** div per component */
inline Matrix<float, 4, 4>& operator /= (Matrix<float, 4, 4>& lhs, float rhs)
{
    __m128 valPacked = _mm_set1_ps(rhs);
    lhs[0].m128      = _mm_div_ps(lhs[0].m128, valPacked);
    lhs[1].m128      = _mm_div_ps(lhs[1].m128, valPacked);
    lhs[2].m128      = _mm_div_ps(lhs[2].m128, valPacked);
    lhs[3].m128      = _mm_div_ps(lhs[3].m128, valPacked);

    return lhs;
}

/** mul matrices */
inline Matrix<float, 4, 4> operator * (const Matrix<float, 4, 4>& lhs, const Matrix<float, 4, 4>& rhs)
{
	Matrix<float, 4, 4> tmp(lhs);
	return tmp *= rhs;
}

/** mul matrix per vertex column */
inline Matrix<float, 4, 1> operator * (const Matrix<float, 4, 4>& mat, const Matrix<float, 4, 1>& vec)
{
#ifdef SIMPLE_GL_USE_SSE4
    __m128 res;
    __m128 dotProd;

    res      = _mm_dp_ps(mat[0].m128, vec.m128, 0xEE);\
    dotProd  = _mm_dp_ps(mat[1].m128, vec.m128, 0xEE);\
    res      = _mm_blend_ps( res, dotProd, _MM_SHUFFLE(0, 1, 1, 1) );\
    dotProd  = _mm_dp_ps(mat[2].m128, vec.m128, 0xEE);\
    res      = _mm_blend_ps( res, dotProd, _MM_SHUFFLE(0, 0, 1, 1) );\
    dotProd  = _mm_dp_ps(mat[3].m128, vec.m128, 0xEE);\
    res      = _mm_blend_ps( res, dotProd, _MM_SHUFFLE(0, 0, 0, 1) );

    return Matrix<float, 4, 1>(res);
#elif defined(SIMPLE_GL_USE_SSE3)
    __m128 res;

    __m128 dotProd0 = _mm_mul_ps(mat[0].m128, vec.m128);
    dotProd0        = _mm_hadd_ps(dotProd0, dotProd0);
    dotProd0        = _mm_hadd_ps(dotProd0, dotProd0);

    __m128 dotProd1 = _mm_mul_ps(mat[1].m128, vec.m128);
    dotProd1        = _mm_hadd_ps(dotProd1, dotProd1);
    dotProd1        = _mm_hadd_ps(dotProd1, dotProd1);

    __m128 dotProd2 = _mm_mul_ps(mat[2].m128, vec.m128);
    dotProd2        = _mm_hadd_ps(dotProd2, dotProd2);
    dotProd2        = _mm_hadd_ps(dotProd2, dotProd2);

    __m128 dotProd3 = _mm_mul_ps(mat[3].m128, vec.m128);
    dotProd3        = _mm_hadd_ps(dotProd3, dotProd3);
    dotProd3        = _mm_hadd_ps(dotProd3, dotProd3);

    __m128 vec01    = _mm_unpacklo_ps(dotProd0, dotProd1);
    __m128 vec23    = _mm_unpackhi_ps(dotProd2, dotProd3);
    res             = _mm_movelh_ps(vec01, vec23);

    return Matrix<float, 4, 1>(res);
#else // SSE2
    // TODO: Think about good sse optimization
    Matrix<float, 4, 1> res;
    res[0] = mat[0][0] * res[0] + mat[0][1] * res[1] + mat[0][2] * res[2] + mat[0][3] * res[3];
    res[1] = mat[1][0] * res[0] + mat[1][1] * res[1] + mat[1][2] * res[2] + mat[1][3] * res[3];
    res[2] = mat[2][0] * res[0] + mat[2][1] * res[1] + mat[2][2] * res[2] + mat[2][3] * res[3];
    res[3] = mat[3][0] * res[0] + mat[3][1] * res[1] + mat[3][2] * res[2] + mat[3][3] * res[3];
    return res;
#endif
}

#endif // SIMPLE_GL_USE_SSE

/** add per component */
template<typename T, int n, int m>
inline Matrix<T, n, m> operator + (const Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
{
    Matrix<T, n, m> tmp(lhs);
    return tmp += rhs;
}

/** unary minus */
template<typename T, int n, int m>
inline Matrix<T, n, m> operator - (const Matrix<T, n, m>& rhs)
{
    Matrix<T, n, m> res;
    for(int i = 0; i<n; ++i)
        res[i] = -rhs[i];
    return res;
}

/** subtract per component */
template<typename T, int n, int m>
Matrix<T, n, m> operator - (const Matrix<T, n, m>& lhs, const Matrix<T, n, m>& rhs)
{
    Matrix<T, n, m> tmp(lhs);
    return tmp -= rhs;
}

/** mul matrices */
template<typename T, int n, int m, int l>
Matrix<T, n, l> operator * (const Matrix<T, n, m>& lhs, const Matrix<T, m, l>& rhs)
{
    Matrix<T, n, l> res;
    for(int i = 0; i<n; ++i)
    {
        for(int j = 0; j<l; ++j)
        {
            res[i][j] = lhs[i][0] * rhs[0][j];
            for(int k = 1; k<m; ++k)
                res[i][j] += lhs[i][k] * rhs[k][j];
        }
    }
    return res;
}

/** mul per component */
template<typename T, int n, int m>
Matrix<T, n, m> operator * (const Matrix<T, n, m>& lhs, T rhs)
{
    Matrix<T, n, m> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n, int m>
Matrix<T, n, m> operator * (T rhs, const Matrix<T, n, m>& lhs)
{
    Matrix<T, n, m> tmp(lhs);
    return tmp *= rhs;
}

/** div per component */
template<typename T, typename Y, int n, int m>
Matrix<T, n, m> operator / (const Matrix<T, n, m>& lhs, Y rhs)
{
    Matrix<T, n, m> tmp(lhs);
    return tmp /= rhs;
}

/** print matrix */
template<typename value_t, int n, int m>
std::ostream& operator << (std::ostream& os, const Matrix<value_t, n, m>& mat)
{
    for(int i = 0; i<n-1; ++i) {
        os << mat[i] << std::endl;
    }

    return os << mat[n-1];
}

/** read matrix */
template<typename value_t, int n, int m>
std::istream& operator >> (std::istream& istream, Matrix<value_t, n, m>& mat)
{
    for(int i = 0; i<n; ++i) {
        istream >> mat[i];
    }

    return istream;
}

/** construct matrix 2x2 */
template<typename T>
Matrix<T, 2, 2> make_matrix( T _0, T _1,
                             T _2, T _3 )
{
    Matrix<T, 2, 2> mat;
    mat[0] = Matrix<T, 1, 2>(_0, _1);
    mat[1] = Matrix<T, 1, 2>(_2, _3);
    return mat;
}

/** construct matrix 3x3 */
template<typename T>
Matrix<T, 3, 3> make_matrix( T _0, T _1, T _2,
                             T _3, T _4, T _5,
                             T _6, T _7, T _8 )
{
    Matrix<T, 3, 3> mat;
    mat[0] = Matrix<T, 1, 3>(_0, _1, _2);
    mat[1] = Matrix<T, 1, 3>(_3, _4, _5);
    mat[2] = Matrix<T, 1, 3>(_6, _7, _8);
    return mat;
}

/** construct matrix 4x4 */
template<typename T>
Matrix<T, 4, 4> make_matrix( T _0,  T _1,  T _2,  T _3,
                             T _4,  T _5,  T _6,  T _7,
                             T _8,  T _9,  T _10, T _11,
                             T _12, T _13, T _14, T _15 )
{
    Matrix<T, 4, 4> mat;
    mat[0] = Matrix<T, 1, 4>(_0,  _1,  _2,  _3);
    mat[1] = Matrix<T, 1, 4>(_4,  _5,  _6,  _7);
    mat[2] = Matrix<T, 1, 4>(_8,  _9,  _10, _11);
    mat[3] = Matrix<T, 1, 4>(_12, _13, _14, _15);
    return mat;
}


/** compare matrix components absolute differences with zero. */
template<typename T, int n, int m>
inline bool fpt_small( const Matrix<T, n, m>& a,
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

/** compare matrix components absolute differences. */
template<typename T, int n, int m>
inline bool fpt_close_abs( const Matrix<T, n, m>& a,
                           const Matrix<T, n, m>& b,
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

/** compare matrix components relative differences. */
template<typename T, int n, int m>
inline bool fpt_close_rel( const Matrix<T, n, m>& a,
                           const Matrix<T, n, m>& b,
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

/** compare matrix components both absolute and relative differences. */
template<typename T, int n, int m>
inline bool fpt_close( const Matrix<T, n, m>& a,
                       const Matrix<T, n, m>& b,
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
template<typename T, int n, int m>
inline Matrix<T, n, m> max(const Matrix<T, n, m>& a, const Matrix<T, n, m>& b)
{
    Matrix<T, n, m> result;
    for(int i = 0; i<n; ++i) {
        result[i] = max(a[i], b[i]);
    }
    return result;
}

/** per component min */
template<typename T, int n, int m>
inline Matrix<T, n, m> min(const Matrix<T, n, m>& a, const Matrix<T, n, m>& b)
{
    Matrix<T, n, m> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min(a[i], b[i]);
    }
    return result;
}

/** per component min & max*/
template<typename T, int n, int m>
inline void min_max( const Matrix<T, n, m>& a,
                     const Matrix<T, n, m>& b,
                     Matrix<T, n, m>&       outMin,
                     Matrix<T, n, m>&       outMax )
{
    for(int i = 0; i<n; ++i) {
        min_max(a[i], b[i], outMin[i], outMax[i]);
    }
}

/** per component clamp */
template<typename T, int n, int m>
inline Matrix<T, n, m> clamp(const Matrix<T, n, m>& mat, T minVal, T maxVal)
{
    Matrix<T, n, m> result;
    for(int i = 0; i<n; ++i) {
        result[i] = clamp(mat[i], minVal, maxVal);
    }
    return result;
}

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

/** Get rotation component from the matrix */
template<typename T>
inline Matrix<T, 3, 3> get_rotation(const math::Matrix<T, 4, 4>& mat)
{
    return make_matrix(mat[0][0], mat[0][1], mat[0][2],
                       mat[1][0], mat[1][1], mat[1][2],
                       mat[2][0], mat[2][1], mat[2][2]);
}

/** Get translation component from the matrix */
template<typename T>
inline Matrix<T, 3, 1> get_translation(const math::Matrix<T, 4, 4>& mat)
{
    return Matrix<T, 3, 1>(mat[0][3], mat[1][3], mat[2][3]);
}

/** Get euler rotation angles from matrix: http://www.geometrictools.com/LibFoundation/Mathematics/Wm4Matrix3.inl.html
 * @param mat - rotation matrix
 * @param res[out] - output angles
 * @return true - if angles can be determined uniquely
 */
template <typename T>
inline bool get_euler_xyz(const Matrix<T, 3, 3>& rot, T& ax, T& ay, T& az)
{
    // +-           -+   +-                                        -+
    // | r00 r01 r02 |   |  cy*cz           -cy*sz            sy    |
    // | r10 r11 r12 | = |  cz*sx*sy+cx*sz   cx*cz-sx*sy*sz  -cy*sx |
    // | r20 r21 r22 |   | -cx*cz*sy+sx*sz   cz*sx+cx*sy*sz   cx*cy |
    // +-           -+   +-                                        -+
    T elem = rot(2);
    if (rot(2) < T(1))
    {
        if (rot(2) > T(-1))
        {
            // y_angle = asin(r02)
            // x_angle = atan2(-r12,r22)
            // z_angle = atan2(-r01,r00)
            ay = asin(rot(2));
            ax = atan2(-rot(5), rot(8));
            az = atan2(-rot(1), rot(0));
            return true;
        }
        else
        {
            // y_angle = -pi/2
            // z_angle - x_angle = atan2(r10,r11)
            // WARNING.  The solution is not unique.  Choosing z_angle = 0.
            ay = -T(HALF_PI);
            ax = -atan2(rot(3), rot(4));
            az = T(0);
            return false;
        }
    }
    else
    {
        // y_angle = +pi/2
        // z_angle + x_angle = atan2(r10,r11)
        // WARNING.  The solutions is not unique.  Choosing z_angle = 0.
        ay = T(HALF_PI);
        ax = atan2(rot(3), rot(4));
        az = T(0);
        return false;
    }
}

/** Get euler rotation angles from matrix: http://www.geometrictools.com/LibFoundation/Mathematics/Wm4Matrix3.inl.html
 * @param mat - rotation matrix
 * @param res[out] - output angles
 * @return true - if angles can be determined uniquely
 */
template <typename T>
inline bool get_euler_xzy(const Matrix<T, 3, 3>& rot, T& ax, T& ay, T& az)
{
    // +-           -+   +-                                        -+
    // | r00 r01 r02 |   |  cy*cz           -sz      cz*sy          |
    // | r10 r11 r12 | = |  sx*sy+cx*cy*sz   cx*cz  -cy*sx+cx*sy*sz |
    // | r20 r21 r22 |   | -cx*sy+cy*sx*sz   cz*sx   cx*cy+sx*sy*sz |
    // +-           -+   +-                                        -+
    if (rot(1) < (T)1)
    {
        if (rot(1) > -(T)1)
        {
            // z_angle = asin(-r01)
            // x_angle = atan2(r21,r11)
            // y_angle = atan2(r02,r00)
            az = (T)asin(-(double)rot(1));
            ax = atan2(rot(7), rot(4));
            ay = atan2(rot(2), rot(0));
            return true;
        }
        else
        {
            // z_angle = +pi/2
            // y_angle - x_angle = atan2(-r20,r22)
            // WARNING.  The solution is not unique.  Choosing y_angle = 0.
            az = HALF_PI;
            ax = -atan2(-rot(6) ,rot(8));
            ay = (T)0;
            return false;
        }
    }
    else
    {
        // z_angle = -pi/2
        // y_angle + x_angle = atan2(-r20,r22)
        // WARNING.  The solution is not unique.  Choosing y_angle = 0.
        az = -HALF_PI;
        ax = atan2(-rot(6), rot(8));
        ay = (T)0;
        return false;
    }
}

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/** Get 3x3 matrix determinant. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix4.inl */
template <typename T>
T determinant(const Matrix<T, 3, 3>& mat)
{
    T co00 = mat(4)*mat(8) - mat(5)*mat(7);
    T co10 = mat(5)*mat(6) - mat(3)*mat(8);
    T co20 = mat(3)*mat(7) - mat(4)*mat(6);

    return mat(0)*co00 + mat(1)*co10 + mat(2)*co20;
}

/** Get 4x4 matrix determinant. Code taken from: http://www.geometrictools.com/LibMathematics/Algebra/Wm5Matrix4.inl */
template<typename T>
T determinant(const Matrix<T, 4, 4>& mat)
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

#endif // SIMPLE_GL_MATH_MATRIX_HPP
