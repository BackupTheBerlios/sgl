#ifndef SIMPLE_GL_MATH_INTERSECTION_HPP
#define SIMPLE_GL_MATH_INTERSECTION_HPP

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

// forward types
template<typename ValueType, INSTRUCTION_SET is>
class AABB;

// forward types
template<typename ValueType, INSTRUCTION_SET is>
class Sphere;

// forward types
template<typename ValueType, INSTRUCTION_SET is>
class Frustum;

// forward types
template<typename ValueType>
class Ray;

/** Check wether this AABB contains another aabb */
template<typename T, INSTRUCTION_SET is>
inline bool contains(const AABB<T, is>& aabb, const AABB<T, is>& other) 
{
    return aabb.minVec.x <= other.minVec.x && aabb.minVec.y <= other.minVec.y && aabb.minVec.z <= other.minVec.z
           && aabb.maxVec.x >= other.maxVec.x && aabb.maxVec.y >= other.maxVec.y && aabb.maxVec.z >= other.maxVec.z;
}

/** Check wether this AABB contains another aabb */
template<>
inline bool contains(const AABB<float, SSE>& aabb, const AABB<float, SSE>& other) 
{
    __m128 minCmp = _mm_cmpge_ps(aabb.maxVec.m128, other.maxVec.m128);
    __m128 maxCmp = _mm_cmple_ps(aabb.minVec.m128, other.minVec.m128);
    __m128 res    = _mm_and_ps(minCmp, maxCmp);
    return res.m128_i32[0] && res.m128_i32[1] && res.m128_i32[2] && res.m128_i32[3];
}

/** Check wether instersection is not empty */
template<typename T, INSTRUCTION_SET is>
inline bool test_intersection( const AABB<T, is>&  a,
                               const AABB<T, is>&  b ) 
{
    // Exit with no intersection if separated along an axis
    if (a.maxVec.x < b.minVec.x || a.minVec.x > b.maxVec.x ) {
        return false;
    }

    if (a.maxVec.x < b.minVec.x || a.minVec.x > b.maxVec.x ) {
        return false;
    }

    if (a.maxVec.x < b.minVec.x || a.minVec.x > b.maxVec.x ) {
        return false;
    }

    return true;
}

/** Check intersection with ray. direction must be normalized */
template<typename T, INSTRUCTION_SET is>
inline bool test_intersection( const AABB<T, is>&  aabb,
                               const Ray<T>&       ray,
                               T                   epsilon = static_cast<T>(EPS_7f) ) 

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

/** Check intersection with ray and find intersection point. direction must be normalized */
template<typename T, INSTRUCTION_SET is>
inline bool find_intersection( const AABB<T, is>&      aabb,
                               const Ray<T>&           ray,
                               Matrix<T, 3, 1, is>&    intersectionPoint,
                               T                       epsilon = static_cast<T>(EPS_7f) )

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

/** Check wether this AABB contains another aabb */
template<typename T, INSTRUCTION_SET is>
inline bool contains(const Sphere<T, is>& a, const Sphere<T, is>& b) 
{
    return length(a.center - b.center) + b.radius <= a.radius;
}

/** Check wether intersection is not empty */
template<typename T, INSTRUCTION_SET is>
inline bool test_intersection( const Sphere<T, is>&  a,
                               const Sphere<T, is>&  b ) 
{
    return length(a.center - b.center) <= a.radius + b.radius;
}

/** Check wether intersection of frustum and BBOX is not empty*/
template<typename T, INSTRUCTION_SET is>
bool test_intersection(const Frustum<T, is>& frustum, const AABB<T, is>& aabb)
{
    Frustum<T, is>::vec3_type vmin;

    for(int i = 0; i<Frustum<T, is>::NUM_PLANES; ++i)
    {
        const Matrix<T, 3, is>& normal = frustum.planes[i].normal;

        // X axis
        vmin.x = normal.x < 0 ? aabb.minVec.x : aabb.maxVec.x;
        vmin.y = normal.y < 0 ? aabb.minVec.y : aabb.maxVec.y;
        vmin.z = normal.z < 0 ? aabb.minVec.z : aabb.maxVec.z;

        if ( dot(normal, vmin) + frustum.planes[i].distance < 0 ) {
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
