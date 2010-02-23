/**
 * \author DikobrAz
 * \date 08.09.09
 * File contains aligned arrays to store sse optimized vectors.
 * Actually used STL containers with aligned allocator if the
 * SSE optimization is enabled and without it otherwise. For the
 * MSVS there is own vector implementation because of compilation
 * bug using vector of aligned elements.
 */

#ifndef SIMPLE_GL_MATH_CONTAINERS_HPP
#define SIMPLE_GL_MATH_CONTAINERS_HPP

#include "../Utility/Aligned.h"
#include "Matrix.hpp"
#include <algorithm>
#include <vector>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

#ifdef SIMPLE_GL_USE_SSE

    #ifdef __GNUC__

    /// Aligned(0x10) std::vector of Vector4f
    typedef std::vector< Vector4f, sgl::aligned_allocator<Vector4f> > vector_of_vector4f;
    /// Aligned(0x10) std::vector of Matrix4f
    typedef std::vector< Matrix4f, sgl::aligned_allocator<Matrix4f> > vector_of_matrix4f;

    #else // MSVS

    /// Aligned(0x10) std::vector of Vector4f
    typedef sgl::vector< Vector4f, sgl::aligned_allocator<Vector4f> > vector_of_vector4f;
    /// Aligned(0x10) std::vector of Matrix4f
    typedef sgl::vector< Matrix4f, sgl::aligned_allocator<Matrix4f> > vector_of_matrix4f;

    #endif // MSVS

#else // SIMPLE_GL_USE_SSE

/// std::vector of Vector4f
typedef std::vector<Vector4f> vector_of_vector4f;
/// std::vector of Matrix4f
typedef std::vector<Matrix4f> vector_of_matrix4f;

#endif // SIMPLE_GL_USE_SSE

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_CONTAINERS_HPP
