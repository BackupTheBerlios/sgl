#ifndef SIMPLE_GL_MATH_INTERSECTION_HPP
#define SIMPLE_GL_MATH_INTERSECTION_HPP

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

// forward types
template<typename ValueType, int n>
class AABB;

// forward types
template<typename ValueType>
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

/** Check wether this AABB contains another aabb */
template<>
inline bool contains(const AABB<float, 4>& aabb, const AABB<float, 4>& other) 
{
    __m128 minCmp = _mm_cmpge_ps(aabb.maxVec.m128, other.maxVec.m128);
    __m128 maxCmp = _mm_cmple_ps(aabb.minVec.m128, other.minVec.m128);
    __m128 res    = _mm_and_ps(minCmp, maxCmp);
    return res.m128_i32[0] && res.m128_i32[1] && res.m128_i32[2] && res.m128_i32[3];
}

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
template<typename T>
inline bool test_intersection( const AABB<T, 3>&    aabb,
                               const Ray<T, 3>&     ray,
                               T                    epsilon = static_cast<T>(EPS_7f) ) 

{
    T tMin = -std::numeric_limits<T>::max();
    T tMax =  std::numeric_limits<T>::max();
    for(int i = 0; i<3; ++i)
    {
        if ( std::abs(ray.get_direction()[i]) > epsilon )
        {
            T t1 = (minVec[i] - ray.get_origin()[i]) / ray.get_direction()[i];
            T t2 = (maxVec[i] - ray.get_origin()[i]) / ray.get_direction()[i];

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
template<typename T>
inline bool test_intersection( const AABB<T, 3>&        aabb,
                               const Matrix<T, 3, 1>&   a,
                               const Matrix<T, 3, 1>&   b,
                               T                        epsilon = static_cast<T>(EPS_7f) ) 

{
    return test_intersection(aabb, math::Ray<T>(a, b-a), epsilon) && test_intersection(aabb, math::Ray<T>(b, a-b), epsilon);
}

template<typename T>
bool find_intersection( const Plane<T, 3>&  plane, 
                        const Ray<T, 3>&    ray, 
                        Matrix<T, 3, 1>&    out,
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
template<typename T>
inline bool find_intersection( const AABB<T, 3>&   aabb,
                               const Ray<T, 3>&    ray,
                               Matrix<T, 3, 1>&    intersectionPoint,
                               T                   epsilon = static_cast<T>(EPS_7f) )

{
    T tMin = -std::numeric_limits<value_type>::max();
    T tMax =  std::numeric_limits<value_type>::max();
    for(int i = 0; i<3; ++i)
    {
        if ( std::abs(ray.get_direction()[i]) > epsilon )
        {
            T t1 = (minVec[i] - ray.get_origin()[i]) / ray.get_direction()[i];
            T t2 = (maxVec[i] - ray.get_origin()[i]) / ray.get_direction()[i];

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

    intersectionPoint = ray.get_origin() + ray.get_direction() * tMin;

    return tMax >= 0;
}

/** Find intersection with segment */
template<typename T>
inline bool find_intersection( const AABB<T, 3>&        aabb,
                               const Matrix<T, 3, 1>&   a,
                               const Matrix<T, 3, 1>&   b,
                               Matrix<T, 3, 1>&         intersectionPoint,
                               T                        epsilon = static_cast<T>(EPS_7f) ) 

{
    return test_intersection(aabb, math::Ray<T>(a, b-a), epsilon) && find_intersection(aabb, math::Ray<T>(b, a-b), epsilon);
}

/** Check wether this AABB contains another aabb */
template<typename T>
inline bool contains(const Sphere<T>& a, const Sphere<T>& b) 
{
    return length(a.center - b.center) + b.radius <= a.radius;
}

/** Check wether intersection is not empty */
template<typename T>
inline bool test_intersection( const Sphere<T>&  a,
                               const Sphere<T>&  b ) 
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
