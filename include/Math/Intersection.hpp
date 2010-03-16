#ifndef SIMPLE_GL_MATH_INTERSECTION_HPP
#define SIMPLE_GL_MATH_INTERSECTION_HPP

#include <limits>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

// forward types
template<typename ValueType, int n>
class AABB;

// forward types
template<typename ValueType, int n>
class Sphere;

// forward types
template<typename ValueType, int n, int m>
class KDop;

// forward types
template<typename ValueType, int n>
class Ray;

// forward type
template<typename ValueType, int n>
class Plane;

/** Check wether Sphere contains point */
template<typename T, int n>
inline bool contains(const Sphere<T, n>& sphere, const Matrix<T, n, 1>& point)
{
    return length(sphere.center - point) < sphere.radius;
}

/** Check wether Sphere contains sphere */
template<typename T, int n>
inline bool contains(const Sphere<T, n>& sphere, const Sphere<T, n>& other)
{
    return length(sphere.center - other.center) + other.radius < sphere.radius;
}

/** Check wether this AABB contains point */
template<typename T, int n>
inline bool contains(const AABB<T, n>& aabb, const typename AABB<T, n>::vec_type& point)
{
    for (int i = 0; i<n; ++i)
    {
        if (point[i] < aabb.minVec[i] || point[i] > aabb.maxVec[i]) {
            return false;
        }
    }

    return true;
}

/** Check wether this AABB contains another aabb */
template<typename T, int n>
inline bool contains(const AABB<T, n>& aabb, const AABB<T, n>& other)
{
    for (int i = 0; i<n; ++i)
    {
        if (other.minVec[i] < aabb.minVec[i] || other.maxVec[i] > aabb.maxVec[i]) {
            return false;
        }
    }

    return true;
}
/*
#ifdef SIMPLE_GL_USE_SSE
template<>
inline bool contains(const AABB<float, 4>& aabb, const AABB<float, 4>& other)
{
    __m128 minCmp = _mm_cmpge_ps(aabb.maxVec.m128, other.maxVec.m128);
    __m128 maxCmp = _mm_cmple_ps(aabb.minVec.m128, other.minVec.m128);
    __m128 res    = _mm_and_ps(minCmp, maxCmp);
    return res.m128_i32[0] && res.m128_i32[1] && res.m128_i32[2] && res.m128_i32[3];
}
#endif
*/
/** Check wether instersection is not empty */
template<typename T, int n>
inline bool test_intersection( const AABB<T, n>&  a,
                               const AABB<T, n>&  b )
{
    for (int i = 0; i<n; ++i)
    {
        if (a.maxVec[i] < b.minVec[i] || a.minVec[i] > b.maxVec[i]) {
            return false;
        }
    }

    return true;
}

/** Check intersection with ray */
template<typename T, int n>
inline bool test_intersection( const AABB<T, n>&    aabb,
                               const Ray<T, n>&     ray,
                               T                    epsilon = static_cast<T>(1e-7) )

{
    T tMin = -std::numeric_limits<T>::max();
    T tMax =  std::numeric_limits<T>::max();
    for(int i = 0; i<n; ++i)
    {
        if ( std::abs(ray.direction[i]) > epsilon )
        {
            T t1 = (aabb.minVec[i] - ray.origin[i]) / ray.direction[i];
            T t2 = (aabb.maxVec[i] - ray.origin[i]) / ray.direction[i];

            if (t1 > t2) {
                std::swap(t1, t2);
            }

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) {
                return false;
            }
        }
    }

    return tMax >= 0;
}

/** Check intersection with segment */
template<typename T, int n>
inline bool test_intersection( const AABB<T, n>&        aabb,
                               const Matrix<T, n, 1>&   a,
                               const Matrix<T, n, 1>&   b,
                               T                        epsilon = static_cast<T>(1e-7) )

{
    return test_intersection(aabb, math::Ray<T,n>(a, b-a), epsilon) && test_intersection(aabb, math::Ray<T,n>(b, a-b), epsilon);
}

template<typename T, int n>
bool find_intersection( const Plane<T, n>&  plane,
                        const Ray<T, n>&    ray,
                        Matrix<T, n, 1>&    out,
                        T                   threshold = std::numeric_limits<T>::epsilon() )
{
    T nDotDir = dot(plane.normal, ray.direction);
    if ( nDotDir > -threshold ) {
        return false;
    }

    T nDotOrigin = dot(plane.normal, ray.origin);
    T t = -(nDotOrigin + plane.distance) / nDotDir;
    if ( t < 0.0 ) {
        return false;
    }

    out = ray.origin + ray.direction * t;
    return true;
}

/** Check intersection with ray and find intersection point */
template<typename T, int n>
inline bool find_intersection( const AABB<T, n>&   aabb,
                               const Ray<T, n>&    ray,
                               Matrix<T, n, 1>&    intPoint,
                               T                   epsilon = static_cast<T>(1e-7) )

{
    T tMin = -std::numeric_limits<T>::max();
    T tMax =  std::numeric_limits<T>::max();
    for(int i = 0; i<n; ++i)
    {
        if ( std::abs(ray.direction[i]) > epsilon )
        {
            T t1 = (aabb.minVec[i] - ray.origin[i]) / ray.direction[i];
            T t2 = (aabb.maxVec[i] - ray.origin[i]) / ray.direction[i];

            if (t1 > t2) {
                std::swap(t1, t2);
            }

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) {
                return false;
            }
        }
    }

    intPoint = ray.origin + ray.direction * tMin;
    return tMax >= 0;
}

/** Find intersection with segment */
template<typename T, int n>
inline bool find_intersection( const AABB<T, n>&        aabb,
                               const Matrix<T, n, 1>&   a,
                               const Matrix<T, n, 1>&   b,
                               Matrix<T, n, 1>&         intPoint,
                               T                        epsilon = static_cast<T>(1e-7) )

{
    return test_intersection(aabb, math::Ray<T,n>(a, b-a), epsilon) && find_intersection(aabb, math::Ray<T,n>(b, a-b), intPoint, epsilon);
}

/** Check wether intersection is not empty */
template<typename T, int n>
inline bool test_intersection( const Sphere<T, n>&  a,
                               const Sphere<T, n>&  b )
{
    return length(a.center - b.center) <= a.radius + b.radius;
}

/** Check wether intersection of frustum and BBOX is not empty*/
template<typename T, int m>
bool test_intersection(const KDop<T, 3, m>& kdop, const AABB<T, 3>& aabb)
{
    Matrix<T, 3, 1> vmin;
    for(int i = 0; i<m; ++i)
    {
        const Matrix<T, 3, 1>& normal = kdop.planes[i].normal;

        // X axis
        vmin.x = normal.x < 0 ? aabb.minVec.x : aabb.maxVec.x;
        vmin.y = normal.y < 0 ? aabb.minVec.y : aabb.maxVec.y;
        vmin.z = normal.z < 0 ? aabb.minVec.z : aabb.maxVec.z;

        if ( dot(normal, vmin) + kdop.planes[i].distance < 0 ) {
            return false;
        }
    }

    return true;
}

} // namesapce math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_INTERSECTION_HPP
