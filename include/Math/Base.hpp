#ifndef SIMPLE_GL_MATH_BASE_HPP
#define SIMPLE_GL_MATH_BASE_HPP

#include "../Config.h"
#include "../Utility/Aligned.h"

#ifdef SIMPLE_GL_USE_SSE
#   ifdef MSVC
#       include <mmintrin.h>
#   else // __GNUC__
#       include <xmmintrin.h>
#       ifdef SIMPLE_GL_USE_SSE4
#           include <smmintrin.h>
#       elif defined(SIMPLE_GL_USE_SSE3)
#           include <pmmintrin.h>
#       else // SIMPLE_GL_USE_SSE2
#           include <emmintrin.h>
#       endif // SIMPLE_GL_USE_SSE4
#   endif // __GNUC__
#endif // SIMPLE_GL_USE_SSE

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

template<typename ValueType, 
         int n, 
         int m>
class MatrixBase
{
public:
    typedef ValueType   value_type;

    static const int    num_elements = n * m;
    static const int    num_rows     = n;
    static const int    num_columns  = m;

protected:
    /** Don't delete using MatrixBase. */
    inline ~MatrixBase() {}
};

// Create useless 1x1 matrices
template<typename T>
class MatrixBase<T, 1, 1> {};

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_BASE_HPP
