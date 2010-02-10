#ifndef SIMPLE_GL_MATH_RAY_H
#define SIMPLE_GL_MATH_RAY_H

#include "Matrix.hpp"

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif

namespace math {

/** Ray in the space */
template<typename value_type>
class Ray
{
public:
    typedef Ray<value_type>            this_type;

    typedef Matrix<value_type, 3, 1>   vec3_type;
    typedef Matrix<value_type, 4, 1>    vec4_type;

    typedef Matrix<value_type, 3, 3>   mat3_type;
    typedef Matrix<value_type, 4, 4>   mat4_type;

public:
    Ray() {}

    Ray(const Ray& rhs) :
        origin(rhs.origin),
        direction(rhs.direction)
    {}

    Ray(const vec3_type& _origin, const vec3_type& _direction) :
        origin(_origin),
        direction( normalize(_direction) )
    {}

    /** Set position of the starting point */
    void set_origin(const vec3_type& _origin) { origin = _origin; }

    /** Get position of the starting point */
    const vec3_type& get_origin() const { return origin; }

    /** Set direction of the ray */
    void set_direction(const vec3_type& _direction) { direction = normalize(_direction); }

    /** Get direction of the ray */
    const vec3_type& get_direction() const { return direction; }

    /** Transform ray by matrix */
    this_type& operator *= (const mat3_type& matrix)
    {
        direction *= matrix;
        origin    *= matrix;
        return *this;
    }

    /** Transform ray by matrix */
    this_type& operator *= (const mat4_type& matrix)
    {
        direction = ( matrix * Vector4f(direction, 0.0f) ).xyz();
        origin    = ( matrix * Vector4f(origin, 1.0f) ).xyz();
        return *this;
    }

private:
    vec3_type   origin;
    vec3_type   direction;
};

typedef Ray<float>    Rayf;
typedef Ray<double>   Rayd;

// operators
/** Transform ray by matrix */
template<typename T>
Ray<T> operator * (const Matrix<T,3,3>& matrix, const Ray<T>& ray)
{
    return Ray<T>(ray) *= matrix;
}

/** Transform ray by matrix */
template<typename T>
Ray<T> operator * (const Ray<T>& ray, const Matrix<T,3,3>& matrix)
{
    return Ray<T>(ray) *= matrix;
}

/** Transform ray by matrix */
template<typename T>
Ray<T> operator * (const Matrix<T,4,4>& matrix, const Ray<T>& ray)
{
    return Ray<T>(ray) *= matrix;
}

/** Transform ray by matrix */
template<typename T>
Ray<T> operator * (const Ray<T>& ray, const Matrix<T,4,4>& matrix)
{
    return Ray<T>(ray) *= matrix;
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_RAY_H
