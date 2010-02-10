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

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

template<typename T, INSTRUCTION_SET is>
class Matrix<T, 1, 1, is> {};

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

template<>
class MatrixBase<float, 4, 4, SSE> :
    public sgl::Aligned<0x40> // If the aligment is 0x10 then sizeof(Matrix<float, 4, 4>) == 80
{
public:
    typedef float       value_type;

    static const int    num_elements = 16;
    static const int    num_rows     = 4;
    static const int    num_columns  = 4;

protected:
    /** Don't delete MatrixBase. */
    inline ~MatrixBase() {}
};

/** Matrix 4x4 class. */
template<INSTRUCTION_SET is>
class Matrix<float, 4, 4, is> :
    public MatrixBase<float, 4, 4, is>
{
public:
    typedef MatrixBase<float, 4, 4, is>     base_type;
    typedef Matrix<float, 4, 4, is>         this_type;
    typedef Matrix<float, 1, 4, is>         row_type;

public:
    inline Matrix() {}
    inline Matrix(const this_type& matrix)
    {
        rows[0] = matrix.rows[0];
        rows[1] = matrix.rows[1];
        rows[2] = matrix.rows[2];
        rows[3] = matrix.rows[3];
    }

    template<INSTRUCTION_SET i>
    inline Matrix(const Matrix<float, 4, 4, i>& matrix)
    {
        rows[0] = matrix.rows[0];
        rows[1] = matrix.rows[1];
        rows[2] = matrix.rows[2];
        rows[3] = matrix.rows[3];
    }

    template<typename T, INSTRUCTION_SET i>
    inline explicit Matrix(const Matrix<T, 4, 4, i>& matrix)
    {
        rows[0] = row_type(matrix.rows[0]);
        rows[1] = row_type(matrix.rows[1]);
        rows[2] = row_type(matrix.rows[2]);
        rows[3] = row_type(matrix.rows[3]);
    }

    inline row_type& operator [] (unsigned int i)
    {
        assert(i < 4);
        return rows[i];
    }

    inline const row_type& operator [] (unsigned int i) const
    {
        assert(i < 4);
        return rows[i];
    }

    /** Get plain matrix data */
    inline float* data() { return rows[0].arr; }

    /** Get plain matrix data */
    inline const float* data() const { return rows[0].arr; }

    /** Get item using linear index */
    inline float& operator () (unsigned int i)
    {
        assert(i < 16);
        return *( data() + i );
    }

    /** Get item using linear index */
    inline float operator () (unsigned int i) const
    {
        assert(i < 16);
        return *( data() + i );
    }

public:
    row_type rows[4];
};

#endif // SIMPLE_GL_USE_SSE

/** Matrix class. */
template< typename ValueType,
          int n,
          int m,
          INSTRUCTION_SET is >
class Matrix :
    public MatrixBase<ValueType, n, m, is>
{
public:
    typedef MatrixBase<ValueType, n, m, is>     base_type;
    typedef Matrix<ValueType, n, m, is>         this_type;
    typedef Matrix<ValueType, 1, m, is>         row_type;

public:
    inline Matrix() {}
    inline Matrix(const this_type& matrix)  { std::copy(matrix.rows, matrix.rows + n, rows); }

    template<INSTRUCTION_SET i>
    inline Matrix(const Matrix<ValueType, n, m, i>& matrix)
    {
        std::copy(matrix.rows, matrix.rows + n, rows);
    }

    template<typename T, INSTRUCTION_SET i>
    inline explicit Matrix(const Matrix<T, n, m, is>& matrix)
    {
        std::copy(matrix.rows, matrix.rows + n, rows);
    }

    inline row_type& operator [] (unsigned int i)
    {
        assert(i < n);
        return rows[i];
    }

    inline const row_type& operator [] (unsigned int i) const
    {
        assert(i < n);
        return rows[i];
    }

    /** Get plain matrix data */
    inline ValueType* data() { return rows[0].arr; }

    /** Get plain matrix data */
    inline const ValueType* data() const { return rows[0].arr; }

    /** Get item using linear index */
    inline ValueType& operator () (unsigned int i)
    {
        assert(i < n * m);
        return *( data() + i );
    }

    /** Get item using linear index */
    inline ValueType operator () (unsigned int i) const
    {
        assert(i < n * m);
        return *( data() + i );
    }

private:
    row_type rows[n];
};

// default matrices
typedef Matrix<double, 4, 4, FPU>           Matrix4x4d; 
typedef Matrix<double, 4, 3, FPU>           Matrix4x3d;
typedef Matrix<double, 3, 4, FPU>           Matrix3x4d;
typedef Matrix<double, 3, 3, FPU>           Matrix3x3d;
typedef Matrix<double, 3, 3, FPU>           Matrix3x2d;
typedef Matrix<double, 3, 3, FPU>           Matrix2x3d;
typedef Matrix<double, 3, 3, FPU>           Matrix2x2d;
typedef Matrix<double, 4, 4, FPU>           Matrix4d;   
typedef Matrix<double, 3, 3, FPU>           Matrix3d;
typedef Matrix<double, 2, 2, FPU>           Matrix2d;

#ifdef SIMPLE_GL_USE_SSE
typedef Matrix<float, 4, 4, SSE>            Matrix4x4f;  /// Matrix4f with SSE
typedef Matrix<float, 4, 4, SSE>            Matrix4f;    /// Matrix4f with SSE
#else
typedef Matrix<float, 4, 4, FPU>            Matrix4x4f;  /// Matrix4f without SSE
typedef Matrix<float, 4, 4, FPU>            Matrix4f;    /// Matrix4f without SSE
#endif

typedef Matrix<float, 4, 3, FPU>            Matrix4x3f;
typedef Matrix<float, 3, 4, FPU>            Matrix3x4f;
typedef Matrix<float, 3, 3, FPU>            Matrix3x3f;
typedef Matrix<float, 3, 3, FPU>            Matrix3x2f;
typedef Matrix<float, 3, 3, FPU>            Matrix2x3f;
typedef Matrix<float, 3, 3, FPU>            Matrix2x2f;
typedef Matrix<float, 3, 3, FPU>            Matrix3f;
typedef Matrix<float, 2, 2, FPU>            Matrix2f;

typedef Matrix<int, 4, 4, FPU>              Matrix4x4i; 
typedef Matrix<int, 4, 3, FPU>              Matrix4x3i;
typedef Matrix<int, 3, 4, FPU>              Matrix3x4i;
typedef Matrix<int, 3, 3, FPU>              Matrix3x3i;
typedef Matrix<int, 3, 3, FPU>              Matrix3x2i;
typedef Matrix<int, 3, 3, FPU>              Matrix2x3i;
typedef Matrix<int, 3, 3, FPU>              Matrix2x2i;
typedef Matrix<int, 4, 4, FPU>              Matrix4i;   
typedef Matrix<int, 3, 3, FPU>              Matrix3i;
typedef Matrix<int, 2, 2, FPU>              Matrix2i;

typedef Matrix<unsigned, 4, 4, FPU>         Matrix4x4ui; 
typedef Matrix<unsigned, 4, 3, FPU>         Matrix4x3ui;
typedef Matrix<unsigned, 3, 4, FPU>         Matrix3x4ui;
typedef Matrix<unsigned, 3, 3, FPU>         Matrix3x3ui;
typedef Matrix<unsigned, 3, 3, FPU>         Matrix3x2ui;
typedef Matrix<unsigned, 3, 3, FPU>         Matrix2x3ui;
typedef Matrix<unsigned, 3, 3, FPU>         Matrix2x2ui;
typedef Matrix<unsigned, 4, 4, FPU>         Matrix4ui;   
typedef Matrix<unsigned, 3, 3, FPU>         Matrix3ui;
typedef Matrix<unsigned, 2, 2, FPU>         Matrix2ui;

typedef Matrix<bool, 4, 4, FPU>             Matrix4x4b; 
typedef Matrix<bool, 4, 3, FPU>             Matrix4x3b;
typedef Matrix<bool, 3, 4, FPU>             Matrix3x4b;
typedef Matrix<bool, 3, 3, FPU>             Matrix3x3b;
typedef Matrix<bool, 3, 3, FPU>             Matrix3x2b;
typedef Matrix<bool, 3, 3, FPU>             Matrix2x3b;
typedef Matrix<bool, 3, 3, FPU>             Matrix2x2b;
typedef Matrix<bool, 4, 4, FPU>             Matrix4b;   
typedef Matrix<bool, 3, 3, FPU>             Matrix3b;
typedef Matrix<bool, 2, 2, FPU>             Matrix2b;

/** add per component */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is>& operator += (Matrix<T, n, m, is>& lhs, const Matrix<T, n, m, is>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] += rhs[i];
    return lhs;
}

/** subtract per component */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is>& operator -= (Matrix<T, n, m, is>& lhs, const Matrix<T, n, m, is>& rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] -= rhs[i];
    return lhs;
}

/** mul matrices */
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, n, is>& operator *= (Matrix<T, n, n, is>& lhs, const Matrix<T, n, n, is>& rhs)
{
    Matrix<T, n, n, is> res;
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
template<typename T, int n, INSTRUCTION_SET is>
inline Matrix<T, n, n, is>& operator *= (Matrix<T, n, n, is>& lhs, T rhs)
{
    for (int i = 0; i<n; ++i) {
        lhs[i] *= rhs;
    }

    return lhs;
}

/** div per component */
template<typename T, typename Y, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is>& operator /= (Matrix<T, n, m, is>& lhs, Y rhs)
{
    for (int i = 0; i<n; ++i)
        lhs[i] /= static_cast<T>(rhs);
    return lhs;
}

/** compare matrices */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline bool operator == (const Matrix<T, n, m, is>& lhs, const Matrix<T, n, m, is>& rhs)
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
template<typename T, int n, int m, INSTRUCTION_SET is>
inline bool operator != (const Matrix<T, n, m, is>& lhs, const Matrix<T, n, m, is>& rhs)
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
inline Matrix<float, 4, 4, SSE>& operator += (Matrix<float, 4, 4, SSE>& lhs, const Matrix<float, 4, 4, SSE>& rhs)
{
    lhs[0].m128 = _mm_add_ps(lhs[0].m128, rhs[0].m128);
    lhs[1].m128 = _mm_add_ps(lhs[1].m128, rhs[1].m128);
    lhs[2].m128 = _mm_add_ps(lhs[2].m128, rhs[2].m128);
    lhs[3].m128 = _mm_add_ps(lhs[3].m128, rhs[3].m128);
    return lhs;
}

/** subtract per component */
inline Matrix<float, 4, 4, SSE>& operator -= (Matrix<float, 4, 4, SSE>& lhs, const Matrix<float, 4, 4, SSE>& rhs)
{
    lhs[0].m128 = _mm_sub_ps(lhs[0].m128, rhs[0].m128);
    lhs[1].m128 = _mm_sub_ps(lhs[1].m128, rhs[1].m128);
    lhs[2].m128 = _mm_sub_ps(lhs[2].m128, rhs[2].m128);
    lhs[3].m128 = _mm_sub_ps(lhs[3].m128, rhs[3].m128);
    return lhs;
}

/** mul matrices */
inline Matrix<float, 4, 4, SSE>& operator *= (Matrix<float, 4, 4, SSE>& lhs, const Matrix<float, 4, 4, SSE>& rhs)
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
inline Matrix<float, 4, 4, SSE>& operator *= (Matrix<float, 4, 4, SSE>& lhs, float rhs)
{
    __m128 valPacked = _mm_set1_ps(rhs);
    lhs[0].m128      = _mm_mul_ps(lhs[0].m128, valPacked);
    lhs[1].m128      = _mm_mul_ps(lhs[1].m128, valPacked);
    lhs[2].m128      = _mm_mul_ps(lhs[2].m128, valPacked);
    lhs[3].m128      = _mm_mul_ps(lhs[3].m128, valPacked);

    return lhs;
}

/** div per component */
inline Matrix<float, 4, 4, SSE>& operator /= (Matrix<float, 4, 4, SSE>& lhs, float rhs)
{
    __m128 valPacked = _mm_set1_ps(rhs);
    lhs[0].m128      = _mm_div_ps(lhs[0].m128, valPacked);
    lhs[1].m128      = _mm_div_ps(lhs[1].m128, valPacked);
    lhs[2].m128      = _mm_div_ps(lhs[2].m128, valPacked);
    lhs[3].m128      = _mm_div_ps(lhs[3].m128, valPacked);

    return lhs;
}


/** mul matrix per vertex column */
inline Matrix<float, 4, 1, SSE> operator * (const Matrix<float, 4, 4, SSE>& mat, const Matrix<float, 4, 1, SSE>& vec)
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

    return Matrix<float, 4, 1, SSE>(res);
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

    return Matrix<float, 4, 1, SSE>(res);
#else // SSE2
    // TODO: Think about good sse optimization
    Matrix<float, 4, 1, SSE> res;
    res[0] = mat[0][0] * res[0] + mat[0][1] * res[1] + mat[0][2] * res[2] + mat[0][3] * res[3];
    res[1] = mat[1][0] * res[0] + mat[1][1] * res[1] + mat[1][2] * res[2] + mat[1][3] * res[3];
    res[2] = mat[2][0] * res[0] + mat[2][1] * res[1] + mat[2][2] * res[2] + mat[2][3] * res[3];
    res[3] = mat[3][0] * res[0] + mat[3][1] * res[1] + mat[3][2] * res[2] + mat[3][3] * res[3];
    return res;
#endif
}

#endif // SIMPLE_GL_USE_SSE

/** add per component */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is> operator + (const Matrix<T, n, m, is>& lhs, const Matrix<T, n, m, is>& rhs)
{
    Matrix<T, n, m, is> tmp(lhs);
    return tmp += rhs;
}

/** unary minus */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is> operator - (const Matrix<T, n, m, is>& rhs)
{
    Matrix<T, n, m, is> res;
    for(int i = 0; i<n; ++i)
        res[i] = -rhs[i];
    return res;
}

/** subtract per component */
template<typename T, int n, int m, INSTRUCTION_SET is>
Matrix<T, n, m, is> operator - (const Matrix<T, n, m, is>& lhs, const Matrix<T, n, m, is>& rhs)
{
    Matrix<T, n, m, is> tmp(lhs);
    return tmp -= rhs;
}

/** mul matrices */
template<typename T, int n, int m, int l, INSTRUCTION_SET is>
Matrix<T, n, l, is> operator * (const Matrix<T, n, m, is>& lhs, const Matrix<T, m, l, is>& rhs)
{
    Matrix<T, n, l, is> res;
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
template<typename T, int n, int m, INSTRUCTION_SET is>
Matrix<T, n, m, is> operator * (const Matrix<T, n, m, is>& lhs, T rhs)
{
    Matrix<T, n, m, is> tmp(lhs);
    return tmp *= rhs;
}

/** mul per component */
template<typename T, int n, int m, INSTRUCTION_SET is>
Matrix<T, n, m, is> operator * (T rhs, const Matrix<T, n, m, is>& lhs)
{
    Matrix<T, n, m, is> tmp(lhs);
    return tmp *= rhs;
}

/** div per component */
template<typename T, typename Y, int n, int m, INSTRUCTION_SET is>
Matrix<T, n, m, is> operator / (const Matrix<T, n, m, is>& lhs, Y rhs)
{
    Matrix<T, n, m, is> tmp(lhs);
    return tmp /= rhs;
}

/** print matrix */
template<typename value_t, int n, int m, INSTRUCTION_SET is>
std::ostream& operator << (std::ostream& os, const Matrix<value_t, n, m, is>& mat)
{
    for(int i = 0; i<n-1; ++i) {
        os << mat[i] << std::endl;
    }

    return os << mat[n-1];
}

/** read matrix */
template<typename value_t, int n, int m, INSTRUCTION_SET is>
std::istream& operator >> (std::istream& istream, Matrix<value_t, n, m, is>& mat)
{
    for(int i = 0; i<n; ++i) {
        istream >> mat[i];
    }

    return istream;
}

/** construct matrix 2x2 */
template<typename T>
Matrix<T, 2, 2, FPU> make_matrix( T _0, T _1,
                                  T _2, T _3 )
{
    Matrix<T, 2, 2, FPU> mat;
    mat[0] = Matrix<T, 1, 2, FPU>(_0, _1);
    mat[1] = Matrix<T, 1, 2, FPU>(_2, _3);
    return mat;
}

/** construct matrix 3x3 */
template<typename T>
Matrix<T, 3, 3, FPU> make_matrix( T _0, T _1, T _2,
                                  T _3, T _4, T _5,
                                  T _6, T _7, T _8 )
{
    Matrix<T, 3, 3, FPU> mat;
    mat[0] = Matrix<T, 1, 3>(_0, _1, _2);
    mat[1] = Matrix<T, 1, 3>(_3, _4, _5);
    mat[2] = Matrix<T, 1, 3>(_6, _7, _8);
    return mat;
}

/** construct matrix 4x4 */
template<typename T>
Matrix<T, 4, 4, FPU> make_matrix( T _0,  T _1,  T _2,  T _3,
                                  T _4,  T _5,  T _6,  T _7,
                                  T _8,  T _9,  T _10, T _11,
                                  T _12, T _13, T _14, T _15 )
{
    Matrix<T, 4, 4, FPU> mat;
    mat[0] = Matrix<T, 1, 4, FPU>(_0,  _1,  _2,  _3);
    mat[1] = Matrix<T, 1, 4, FPU>(_4,  _5,  _6,  _7);
    mat[2] = Matrix<T, 1, 4, FPU>(_8,  _9,  _10, _11);
    mat[3] = Matrix<T, 1, 4, FPU>(_12, _13, _14, _15);
    return mat;
}

/** compare to matrices using threshold */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline bool equal( const Matrix<T, n, m, is>& a,
                   const Matrix<T, n, m, is>& b,
                   T threshold = std::numeric_limits<T>::epsilon() )
{
    for(int i = 0; i<n; ++i)
    {
        if ( !equal(a[i] - b[i], threshold) ) {
            return false;
        }
    }

    return true;
}

/** per component max */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is> max(const Matrix<T, n, m, is>& a, const Matrix<T, n, m, is>& b)
{
    Matrix<T, n, m, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = max(a[i], b[i]);
    }
    return result;
}

/** per component min */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is> min(const Matrix<T, n, m, is>& a, const Matrix<T, n, m, is>& b)
{
    Matrix<T, n, m, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = std::min(a[i], b[i]);
    }
    return result;
}

/** per component min & max*/
template<typename T, int n, int m, INSTRUCTION_SET is>
inline void min_max( const Matrix<T, n, m, is>& a,
                     const Matrix<T, n, m, is>& b,
                     Matrix<T, n, m, is>&       outMin,
                     Matrix<T, n, m, is>&       outMax )
{
    for(int i = 0; i<n; ++i) {
        min_max(a[i], b[i], outMin[i], outMax[i]);
    }
}

/** per component clamp */
template<typename T, int n, int m, INSTRUCTION_SET is>
inline Matrix<T, n, m, is> clamp(const Matrix<T, n, m, is>& mat, T minVal, T maxVal)
{
    Matrix<T, n, m, is> result;
    for(int i = 0; i<n; ++i) {
        result[i] = clamp(mat[i], minVal, maxVal);
    }
    return result;
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#ifdef MSVC
#pragma warning( pop )
#endif

#endif // SIMPLE_GL_MATH_MATRIX_HPP
