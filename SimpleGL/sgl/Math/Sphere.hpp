#ifndef SIMPLE_GL_MATH_SPHERE_H
#define SIMPLE_GL_MATH_SPHERE_H

#include "Vector.hpp"

SGL_BEGIN_MATH_NAMESPACE

/** Axis aligned bounding box. */
template<typename ValueType, 
         int      Dimension>
class Sphere
{
public:
    typedef Sphere<ValueType, Dimension>        this_type;
    typedef ValueType                           value_type;
    typedef Matrix<value_type, Dimension, 1>    vec_type;

public:
    inline Sphere() {}
    inline Sphere(const vec_type& center_, value_type radius_) :
        center(center_),
        radius(radius_)
    {}

public:
    vec_type    center;
    value_type  radius;
};

typedef Sphere<float, 2>    Circlef;
typedef Sphere<double, 2>   Circled;

typedef Sphere<float, 3>    Sphere3f;
typedef Sphere<double, 3>   Sphere3d;

typedef Sphere<float, 3>    Spheref;
typedef Sphere<double, 3>   Sphered;

// Compare AABB
template<typename T, int n>
inline bool operator == (const Sphere<T, n>& lhs, const Sphere<T, n>& rhs) 
{ 
    return lhs.center == rhs.center && lhs.radius == rhs.radius; 
}

template<typename T, int n>
inline bool operator != (const Sphere<T, n>& lhs, const Sphere<T, n>& rhs) 
{ 
    return lhs.center != rhs.center || lhs.radius != rhs.radius; 
}

SGL_END_MATH_NAMESPACE

#endif // SIMPLE_GL_MATH_SPHERE_H
