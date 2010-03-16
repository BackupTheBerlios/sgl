#ifndef SIMPLE_GL_MATH_RAY_H
#define SIMPLE_GL_MATH_RAY_H

#include "../Utility/Meta.h"
#include "Matrix.hpp"

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif

namespace math {

/** Ray in the space */
template<typename  ValueType,
         int       Dimension>
class Ray
{
public:
    typedef ValueType                       value_type;
    typedef Ray<value_type, Dimension>      this_type;
    typedef Matrix<ValueType, Dimension, 1> vec_type;

public:
    Ray() {}

    Ray(const this_type& rhs) :
        origin(rhs.origin),
        direction(rhs.direction)
    {}

    Ray(const vec_type& _origin, const vec_type& _direction) :
        origin(_origin),
        direction( normalize(_direction) )
    {}

public:
    vec_type    origin;
    vec_type    direction;
};

typedef Ray<float, 2>   Ray2f;
typedef Ray<double, 2>  Ray2d;

typedef Ray<float, 3>   Ray3f;
typedef Ray<double, 3>  Ray3d;

typedef Ray<float, 4>   Ray4f;
typedef Ray<double, 4>  Ray4d;

typedef Ray<float, 3>   Rayf;
typedef Ray<double, 3>  Rayd;

// operators

template<typename T>
Ray<T, 3> operator *= (Ray<T, 3>& ray, const Matrix<T, 3, 3>& matrix)
{
    ray.direction *= matrix;
    ray.origin    *= matrix;
    return ray;
}

template<typename T>
Ray<T, 4> operator *= (Ray<T, 4>& ray, const Matrix<T, 4, 4>&  matrix)
{
    ray.direction *= matrix;
    ray.origin    *= matrix;
    return ray;
}

template<typename T>
Ray<T, 3> operator * (const Matrix<T, 3, 3>& matrix, const Ray<T, 3>& ray)
{
    Ray<T, 3> tmp(ray);
    return tmp *= matrix;
}

template<typename T>
Ray<T, 4> operator * (const Matrix<T, 4, 4>& matrix, const Ray<T, 4>& ray)
{
    Ray<T, 4> tmp(ray);
    return tmp *= matrix;
}

template<typename T>
Ray<T, 3> operator * (const Ray<T, 3>& ray, const Matrix<T, 3, 3>& matrix)
{
    Ray<T, 3> tmp(ray);
    return tmp *= matrix;
}

template<typename T>
Ray<T, 4> operator * (const Ray<T, 4>& ray, const Matrix<T, 4, 4>& matrix)
{
    Ray<T, 4> tmp(ray);
    return tmp *= matrix;
}

/** Normalize ray direction */
template<typename T, int n>
Ray<T, n> normalize(const Ray<T, n>& ray)
{
    return Ray<T, n>( ray.origin, ray.direction / length(ray.direction) );
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_RAY_H
