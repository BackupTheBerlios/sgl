#ifndef SIMPLE_GL_MATH_SPHERE_H
#define SIMPLE_GL_MATH_SPHERE_H

#include "Ray.hpp"
#include "Utility.hpp"
#include <cmath>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

/** Axis aligned bounding box. */
template<typename ValueType, INSTRUCTION_SET is = DEFAULT_INSTRUCTION_SET>
class Sphere
{
public:
    typedef Sphere<ValueType>               this_type;
    typedef ValueType                       value_type;

    typedef Matrix<value_type, 3, 1, is>    vec_type;
    typedef Matrix<value_type, 3, 1, is>    vec3_type;
    typedef Matrix<value_type, 4, 1, is>    vec4_type;

    typedef Matrix<value_type, 3, 3, is>    mat3_type;
    typedef Matrix<value_type, 4, 4, is>    mat4_type;

public:
    inline Sphere() {}
    inline Sphere(const vec3_type& _center, value_type _radius) :
        center(_center),
        radius(_radius)
    {}

public:
    vec_type    center;
    value_type  radius;
};

typedef Sphere<float>     Spheref;
typedef Sphere<double>    Sphered;

// operators

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline Sphere<T, is>& operator *= (Sphere<T, is>& sphere, const Matrix<T, 3, 3, is>& matrix)
{
    sphere.center = matrix * sphere.center;
    sphere.radius = length( matrix * Vector<T, 3, 1, is>(radius, 0, 0) );
    return aabb;
}

/** Transform plane by matrix */
template<typename T, INSTRUCTION_SET is>
inline Sphere<T, is>& operator *= (Sphere<T, is>& sphere, const Matrix<T, 4, 4, is>& matrix)
{
    sphere.center = matrix * sphere.center;
    sphere.radius = length( matrix * Vector<T, 4, 1, is>(radius, 0, 0, 0) );
    return aabb;
}

// Compare AABB
template<typename T, INSTRUCTION_SET is>
inline bool operator == (const Sphere<T, is>& lhs, const Sphere<T, is>& rhs) 
{ 
    return lhs.center == rhs.center && lhs.radius == rhs.radius; 
}

template<typename T, INSTRUCTION_SET is>
inline bool operator != (const Sphere<T, is>& lhs, const Sphere<T, is>& rhs) 
{ 
    return lhs.center != rhs.center || lhs.radius != rhs.radius; 
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline Sphere<T, is> operator * (const Matrix<T, 3, 3, is>& matrix, const Sphere<T, is>& aabb)
{
    return Sphere<T, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline Sphere<T, is> operator * (const Sphere<T, is>& aabb, const Matrix<T, 3, 3, is>& matrix)
{
    return Sphere<T, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline Sphere<T, is> operator * (const Matrix<T, 4, 4, is>& matrix, const Sphere<T, is>& aabb)
{
    return Sphere<T, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline Sphere<T, is> operator * (const Sphere<T, is>& aabb, const Matrix<T, 4, 4, is>& matrix)
{
    return Sphere<T, is>(aabb) *= matrix;
}

/** Make minimum size sphere containing 2 spheres */
template<typename T, INSTRUCTION_SET is>
inline Sphere<T, is> merge(const Sphere<T, is>& a, const Sphere<T, is>& b)
{
    // find radius
    T radius, dist;
    dist   = length(a.center - b.center);
    radius = T(0.5) * (dist + a.radius + b.radius);
    if (radius < a.radius) {
        return a; // a contains b
    }
    else if (radius < b.radius) {
        return b; // b contains a
    }

    // find center
    Matrix<T, 3, 1, is> center = ( a.center * (dist - T(1.0)) + b.center * (radius - a.radius) ) / dist;
    return Sphere<T, is>(center, radius);
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_SPHERE_H
