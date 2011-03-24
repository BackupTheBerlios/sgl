#ifndef SIMPLE_GL_MATH_PLANE_H
#define SIMPLE_GL_MATH_PLANE_H

#include "Matrix.hpp"

SGL_BEGIN_MATH_NAMESPACE

template<typename  ValueType,
         int       Dimension>
class Plane
{
public:
    typedef ValueType                               value_type;
    typedef Plane<value_type, Dimension>            this_type;
    typedef Matrix<value_type, Dimension, 1>        vec_type;
    typedef Matrix<value_type, Dimension + 1, 1>    vec_plus_type;

public:
    Plane() {}

    Plane(const Plane& rhs) :
        normal(rhs.normal),
        distance(rhs.distance)
    {}

    Plane(const vec_type& normal_, value_type distance_) :
        normal(normal_),
        distance(distance_)
    {}

    Plane(value_type A, value_type B, value_type C, value_type D)
    {
        value_type l = ::sqrt(A*A + B*B + C*C + D*D);
        distance = D;
        normal   = vec_type(A, B, C);
    }

    Plane(const vec_type& a, const vec_type& b, const vec_type& c)
    {
        normal   = normalize( cross(b - a, c - a) );
        distance = -dot(a, normal);
    }

    /** Represent plane as vector */
    vec_plus_type as_vec() const { return make_vec(normal, distance); }

    /** Reflect vector from the plane */
    vec_type reflect(const vec_type& vec) const {
        return vec - 2 * (dot(vec, normal) + distance) * normal;
    }

    /** Reflect vector from the plane normal */
    vec_type reflect_direction(const vec_type& vec) const {
        return vec - 2 * dot(vec, normal) * normal;
    }

public:
    vec_type    normal;
    value_type  distance;
};

typedef Plane<float,  2>    Linef;
typedef Plane<double, 2>    Lined;

typedef Plane<float,  3>    Plane3f;
typedef Plane<double, 3>    Plane3d;

typedef Plane<float,  3>    Planef;
typedef Plane<double, 3>    Planed;

// operators
template<typename T, int n>
Plane<T, n>& operator *= (Plane<T, n>& plane, const Matrix<T, n, n>& matrix)
{
    plane.normal = matrix * plane.normal;
    return plane;
}

template<typename T, int n>
Plane<T, n>& operator *= (Plane<T, n>& plane, const Matrix<T, n+1, n+1>& matrix)
{
    Matrix<T, n+1, 1> planeVec = matrix * make_vec( -plane.normal * plane.distance, static_cast<T>(1) );
    plane.normal               = cdr( matrix * make_vec( plane.normal, static_cast<T>(0) ) );
    plane.distance             = -dot( plane.normal, cdr(planeVec) );

    return plane;
}

template<typename T, int n>
Plane<T, n> operator * (const Matrix<T,n,n>& matrix, const Plane<T, n>& plane)
{
    Plane<T, n> tmp(plane);
    return tmp *= matrix;
}

template<typename T, int n>
Plane<T, n> operator * (const Plane<T, n>& plane, const Matrix<T,n,n>& matrix)
{
    Plane<T, n> tmp(plane);
    return tmp *= matrix;
}

template<typename T, int n>
Plane<T, n> operator * (const Matrix<T,n+1,n+1>& matrix, const Plane<T, n>& plane)
{
    Plane<T, n> tmp(plane);
    return tmp *= matrix;
}

template<typename T, int n>
Plane<T, n> operator * (const Plane<T, n>& plane, const Matrix<T,n+1,n+1>& matrix)
{
    Plane<T, n> tmp(plane);
    return tmp *= matrix;
}

template<typename T, int n>
Plane<T, n> normalize(const Plane<T, n>& plane)
{
    T l = length(plane.normal);
    return Plane<T, n>(plane.normal / l, plane.distance / l);
}

SGL_END_MATH_NAMESPACE

#endif // SIMPLE_GL_MATH_PLANE_H
