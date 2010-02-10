#ifndef SIMPLE_GL_MATH_PLANE_H
#define SIMPLE_GL_MATH_PLANE_H

#include "Ray.hpp"

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

template<typename ValueType>
class Plane
{
public:
    typedef ValueType                   value_type;
    typedef Plane<value_type>           this_type;
    
    typedef Ray<value_type>             ray_type;

    typedef Matrix<value_type, 3, 1>    vec3_type;
    typedef Matrix<value_type, 4, 1>    vec4_type;

    typedef Matrix<value_type, 3, 3>    mat3_type;
    typedef Matrix<value_type, 4, 4>    mat4_type;

private:
    // FIXME: Use something except define. Using static constant arise error in MSVS
    #define _0 static_cast<ValueType>(0)
    #define _1 static_cast<ValueType>(1)

public:
    Plane() {}
    
    Plane(const Plane& rhs) :
        normal(rhs.normal),
        distance(rhs.distance)
    {}

    Plane(const vec3_type& _normal, value_type _distance) :
        normal(_normal),
        distance(_distance)
    {}

    Plane(value_type A, value_type B, value_type C, value_type D)
    {
        value_type l = ::sqrt(A*A + B*B + C*C + D*D);
        distance = D;
        normal   = vec3_type(A, B, C);
    }

    Plane(const vec3_type& a, const vec3_type& b, const vec3_type& c)
    {
        normal   = normalize( cross(b - a, c - a) );
        distance = -dot(a, normal); 
    }

    /** Represent plane as 4 dimensional vector */
    vec4_type as_vec() const { return make_vec(normal, distance); }

    /** Reflect vector from the plane */
    vec3_type reflect(const Matrix<value_type, 3, 1>& vec) const {
        return vec - 2 * (dot(vec, normal) + distance) * normal;
    }

    /** Reflect vector from the plane */
    vec4_type reflect(const Matrix<value_type, 4, 1>& vec) const 
    {
        vec4_type normal4(normal.x, normal.y, normal.z, 0.0);
        return vec - 2 * (dot(vec, normal4) + distance) * normal4;
    }

    /** Reflect vector from the plane normal */
    vec3_type reflect_direction(const Matrix<value_type, 3, 1>& vec) const {
        return vec - 2 * dot(vec, normal) * normal;
    }

    /** Get intersection with the ray */
    bool find_intersection( const ray_type& ray, 
                            vec3_type&      out,
                            value_type      threshold = std::numeric_limits<value_type>::epsilon() ) const
    {
        value_type nDotDir = dot( normal, ray.get_direction() );
        if ( nDotDir > -threshold ) {
            return false;
        }

        value_type nDotOrigin = dot( normal, ray.get_origin() ); 
        value_type t = -(nDotOrigin + distance) / nDotDir;
        if ( t < 0.0 ) {
            return false;
        }

        out = ray.get_origin() + ray.get_direction() * t;
        return true;
    }

    /** Transform plane by matrix */
    this_type& operator *= (const mat3_type& matrix)
    {
        normal *= matrix;
        return *this;
    }

    /** Transform plane by matrix */
    this_type& operator *= (const mat4_type& matrix)
    {
        vec4_type plane = matrix * make_vec(-normal * distance, _1);
        normal   = xyz( matrix * make_vec(normal, _0) );
        distance = -dot( normal, xyz(plane) );
        return *this;
    }

#undef _0
#undef _1

public:
    vec3_type   normal;
    value_type  distance;
};

typedef Plane<float>    Planef;
typedef Plane<double>   Planed;

// operators
/** Transform plane by matrix */
template<typename T>
Plane<T> operator * (const Matrix<T,3,3>& matrix, const Plane<T>& plane)
{
    return Plane<T>(plane) *= matrix;
}

/** Transform plane by matrix */
template<typename T>
Plane<T> operator * (const Plane<T>& plane, const Matrix<T,3,3>& matrix)
{
    return Plane<T>(plane) *= matrix;
}

/** Transform plane by matrix */
template<typename T>
Plane<T> operator * (const Matrix<T,4,4>& matrix, const Plane<T>& plane)
{
    return Plane<T>(plane) *= matrix;
}

/** Transform plane by matrix */
template<typename T>
Plane<T> operator * (const Plane<T>& plane, const Matrix<T,4,4>& matrix)
{
    return Plane<T>(plane) *= matrix;
}

/** Normalize plane */
template<typename T>
Plane<T> normalize(const Plane<T>& plane)
{
    T l = length(plane.normal);
    return Plane<T>(plane.normal / l, plane.distance / l);
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_PLANE_H
