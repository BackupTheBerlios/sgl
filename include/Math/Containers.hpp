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
#include "Quaternion.hpp"
#include <algorithm>
#include <vector>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

#ifdef SIMPLE_GL_USE_SSE

    #ifdef __GNUC__

    /// Aligned(0x10) std::vector of Vector4f
    typedef std::vector< Vector4f, sgl::aligned_allocator<Vector4f> >       vector_of_vector4f;
    /// Aligned(0x10) std::vector of Matrix4f
    typedef std::vector< Matrix4f, sgl::aligned_allocator<Matrix4f> >       vector_of_matrix4f;
    /// Aligned(0x10) std::vector of Quaternionf
    typedef sgl::vector< Quaternionf, sgl::aligned_allocator<Quaternionf> > vector_of_quaternionf;

    #else // MSVS

    /// Aligned(0x10) std::vector of Vector4f
    typedef sgl::vector< Vector4f, sgl::aligned_allocator<Vector4f> >       vector_of_vector4f;
    /// Aligned(0x10) std::vector of Matrix4f
    typedef sgl::vector< Matrix4f, sgl::aligned_allocator<Matrix4f> >       vector_of_matrix4f;
    /// Aligned(0x10) std::vector of Quaternionf
    typedef sgl::vector< Quaternionf, sgl::aligned_allocator<Quaternionf> > vector_of_quaternionf;

    #endif // MSVS

#else // SIMPLE_GL_USE_SSE

/// std::vector of Vector4f
typedef std::vector<Vector4f>       vector_of_vector4f;
/// std::vector of Matrix4f
typedef std::vector<Matrix4f>       vector_of_matrix4f;
/// std::vector of Quaternionf
typedef std::vector<Quaternionf>    vector_of_quaternionf;

#endif // SIMPLE_GL_USE_SSE

/// std::vector of Vector3f
typedef std::vector<Vector3f> vector_of_vector3f;
/// std::vector of Vector2f
typedef std::vector<Vector2f> vector_of_vector2f;

/// std::vector of Vector4d
typedef std::vector<Vector4d> vector_of_vector4d;
/// std::vector of Vector3d
typedef std::vector<Vector3d> vector_of_vector3d;
/// std::vector of Vector2d
typedef std::vector<Vector2d> vector_of_vector2d;

/// std::vector of Vector4i
typedef std::vector<Vector4i> vector_of_vector4i;
/// std::vector of Vector3i
typedef std::vector<Vector3i> vector_of_vector3i;
/// std::vector of Vector2i
typedef std::vector<Vector2i> vector_of_vector2i;

/// std::vector of Vector4ui
typedef std::vector<Vector4ui> vector_of_vector4ui;
/// std::vector of Vector3ui
typedef std::vector<Vector3ui> vector_of_vector3ui;
/// std::vector of Vector2ui
typedef std::vector<Vector2ui> vector_of_vector2ui;

/// std::vector of Vector4b
typedef std::vector<Vector4b> vector_of_vector4b;
/// std::vector of Vector3b
typedef std::vector<Vector3b> vector_of_vector3b;
/// std::vector of Vector2b
typedef std::vector<Vector2b> vector_of_vector2b;

/// std::vector of Matrix3f
typedef std::vector<Matrix3f> vector_of_matrix3f;
/// std::vector of Matrix2f
typedef std::vector<Matrix2f> vector_of_matrix2f;

/// std::vector of Matrix4d
typedef std::vector<Matrix4d> vector_of_matrix4d;
/// std::vector of Matrix3d
typedef std::vector<Matrix3d> vector_of_matrix3d;
/// std::vector of Matrix2d
typedef std::vector<Matrix2d> vector_of_matrix2d;

/// std::vector of Matrix4i
typedef std::vector<Matrix4i> vector_of_matrix4i;
/// std::vector of Matrix3i
typedef std::vector<Matrix3i> vector_of_matrix3i;
/// std::vector of Matrix2i
typedef std::vector<Matrix2i> vector_of_matrix2i;

/// std::vector of Matrix4ui
typedef std::vector<Matrix4ui> vector_of_matrix4ui;
/// std::vector of Matrix3ui
typedef std::vector<Matrix3ui> vector_of_matrix3ui;
/// std::vector of Matrix2ui
typedef std::vector<Matrix2ui> vector_of_matrix2ui;

/// std::vector of Matrix4b
typedef std::vector<Matrix4b> vector_of_matrix4b;
/// std::vector of Matrix3b
typedef std::vector<Matrix3b> vector_of_matrix3b;
/// std::vector of Matrix2b
typedef std::vector<Matrix2b> vector_of_matrix2b;

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_CONTAINERS_HPP
