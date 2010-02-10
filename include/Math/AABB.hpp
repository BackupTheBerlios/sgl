#ifndef SIMPLE_GL_MATH_AABB_H
#define SIMPLE_GL_MATH_AABB_H

#include "Ray.hpp"
#include "MatrixFunctions.hpp"
#include "Utility.hpp"
#include <cmath>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

/** Axis aligned bounding box. */
template<typename ValueType, INSTRUCTION_SET is = DEFAULT_INSTRUCTION_SET>
class AABB
{
public:
    typedef AABB<ValueType>                 this_type;
    typedef ValueType                       value_type;

    typedef Matrix<value_type, 3, 1, is>    vec_type;
    typedef Matrix<value_type, 3, 1, is>    vec3_type;
    typedef Matrix<value_type, 4, 1, is>    vec4_type;

    typedef Matrix<value_type, 3, 3, is>    mat3_type;
    typedef Matrix<value_type, 4, 4, is>    mat4_type;

    typedef Ray<value_type>                 ray_type;

private:
    // FIXME: Use something except define. Using static constant arise error in MSVS
    #define _0 static_cast<value_type>(0)
    #define _1 static_cast<value_type>(1)

public:
    inline AABB() {}
    inline AABB(const vec3_type& _minVec, const vec3_type& _maxVec) :
        minVec(_minVec),
        maxVec(_maxVec)
    {}

    inline AABB(const vec4_type& _minVec, const vec4_type& _maxVec) :
        minVec( xyz(_minVec) ),
        maxVec( xyz(_maxVec) )
    {}

    inline AABB( value_type minx, 
                 value_type miny, 
                 value_type minz, 
                 value_type maxx, 
                 value_type maxy, 
                 value_type maxz ) :
        minVec(minx, miny, minz),
        maxVec(maxx, maxy, maxz)
    {
    }

    /** Setup aabb using top right far corner and bottom left near corner */
    inline AABB& setup(const vec3_type& _minVec, const vec3_type& _maxVec)
    {
        minVec = _minVec;
        maxVec = _maxVec;

        return *this;
    }

    /** Setup aabb using top right far corner and bottom left near corner */
    inline AABB& setup(const vec4_type& _minVec, const vec4_type& _maxVec)
    {
        minVec = xyz(_minVec);
        maxVec = xyz(_maxVec);

        return *this;
    }

    /** Extend aabb so point will be covered */
    inline AABB& extend(const vec3_type& vec)
    {
        return extend( make_vec(vec, _1) );
    }

    /** Extend aabb so point will be covered */
    inline AABB& extend(const vec4_type& vec)
    {
        minVec = min(vec, minVec);
        maxVec = max(vec, maxVec);

        return *this;
    }

    /** Get size of the box */
    inline vec3_type size() const  { return maxVec - minVec; }

    /** Get center of the box */
    inline vec3_type center() const  { return (maxVec + minVec) * static_cast<value_type>(0.5); }

#undef _0
#undef _1

public:
    vec_type minVec;
    vec_type maxVec;
};

/** Axis aligned bounding box sse optimized. */
template<>
class AABB<float, SSE>
    : public sgl::Aligned16
{
public:
    typedef AABB<float>                     this_type;
    typedef float                           value_type;

    typedef Matrix<value_type, 4, 1, SSE>    vec_type;
    typedef Matrix<value_type, 3, 1, SSE>    vec3_type;
    typedef Matrix<value_type, 4, 1, SSE>    vec4_type;

    typedef Matrix<value_type, 3, 3, SSE>    mat3_type;
    typedef Matrix<value_type, 4, 4, SSE>    mat4_type;

    typedef Ray<value_type>                 ray_type;

private:
    // FIXME: Use something except define. Using static constant arise error in MSVS
    #define _0 static_cast<value_type>(0)
    #define _1 static_cast<value_type>(1)

public:
    inline AABB() {}
    inline AABB(const vec3_type& _minVec, const vec3_type& _maxVec) :
        minVec( make_vec(_minVec, _1) ),
        maxVec( make_vec(_maxVec, _1) )
    {}

    inline AABB(const vec4_type& _minVec, const vec4_type& _maxVec) :
        minVec(_minVec),
        maxVec(_maxVec)
    {}

    inline AABB( value_type minx, 
                 value_type miny, 
                 value_type minz, 
                 value_type maxx, 
                 value_type maxy, 
                 value_type maxz ) :
        minVec(minx, miny, minz, _1),
        maxVec(maxx, maxy, maxz, _1)
    {
    }

    /** Setup aabb using top right far corner and bottom left near corner */
    inline AABB& setup(const vec3_type& _minVec, const vec3_type& _maxVec)
    {
        minVec = make_vec(_minVec, _1);
        maxVec = make_vec(_maxVec, _1);

        return *this;
    }

    /** Setup aabb using top right far corner and bottom left near corner */
    inline AABB& setup(const vec4_type& _minVec, const vec4_type& _maxVec)
    {
        minVec = _minVec;
        maxVec = _maxVec;

        return *this;
    }

    /** Extend aabb so point will be covered */
    inline AABB& extend(const vec3_type& vec)
    {
        return extend( make_vec(vec, _1) );
    }

    /** Extend aabb so point will be covered */
    inline AABB& extend(const vec4_type& vec)
    {
        minVec = min(vec, minVec);
        maxVec = max(vec, maxVec);

        return *this;
    }

    /** Get size of the box */
    inline vec3_type size() const  { return xyz(maxVec - minVec); }

    /** Get center of the box */
    inline vec3_type center() const  { return xyz( (maxVec + minVec) * static_cast<value_type>(0.5) ); }

#undef _0
#undef _1

public:
    vec_type minVec;
    vec_type maxVec;
};

typedef AABB<float>     AABBf;
typedef AABB<double>    AABBd;

// operators

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is>& operator *= (AABB<T, is>& aabb, const Matrix<T, 3, 3, is>& matrix)
{
    AABB<T, is>::vec_type   aMin, aMax;
    float                   a, b;

    // Copy box A into min and max array.
    aMin = aabb.minVec;
    aMax = aabb.maxVec;

    // Begin at translation
    aabb.minVec.x = aabb.maxVec.x = T(0);
    aabb.minVec.y = aabb.maxVec.y = T(0);
    aabb.minVec.z = aabb.maxVec.z = T(0);

    // Find extreme points by considering product of 
    // min and max with each component of M.
    for(int j=0; j<3; ++j)
    {
        for(int i=0; i<3; ++i)
        {
            a = matrix[i][j] * aMin[i];
            b = matrix[i][j] * aMax[i];

            if( a < b )
            {
                aabb.minVec[j] += a;
                aabb.maxVec[j] += b;
            }
            else
            {
                aabb.minVec[j] += b;
                aabb.maxVec[j] += a;
            }
        }
    }

    return aabb;
}

/** Transform plane by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is>& operator *= (AABB<T, is>& aabb, const Matrix<T, 4, 4, is>& matrix)
{
    AABB<T, is>::vec_type   aMin, aMax;
    float                   a, b;

    // Copy box A into min and max array.
    aMin = aabb.minVec;
    aMax = aabb.maxVec;

    // Begin at translation
    AABB<T, is>::vec3_type trans = get_translation(matrix);
    aabb.minVec.x = aabb.maxVec.x = trans.x;
    aabb.minVec.y = aabb.maxVec.y = trans.y;
    aabb.minVec.z = aabb.maxVec.z = trans.z;


    // Find extreme points by considering product of 
    // min and max with each component of M.
    for(int j=0; j<3; ++j)
    {
        for(int i=0; i<3; ++i)
        {
            a = matrix[i][j] * aMin[i];
            b = matrix[i][j] * aMax[i];

            if( a < b )
            {
                aabb.minVec[j] += a;
                aabb.maxVec[j] += b;
            }
            else
            {
                aabb.minVec[j] += b;
                aabb.maxVec[j] += a;
            }
        }
    }

    return aabb;
}

// Compare AABB
template<typename T, INSTRUCTION_SET is>
inline bool operator == (const AABB<T, is>& lhs, const AABB<T, is>& rhs) 
{ 
    return lhs.minVec == rhs.minVec && lhs.maxVec == rhs.maxVec; 
}

template<typename T, INSTRUCTION_SET is>
inline bool operator != (const AABB<T, is>& lhs, const AABB<T, is>& rhs) 
{ 
    return lhs.minVec != rhs.minVec || lhs.maxVec != rhs.maxVec; 
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is> operator * (const Matrix<T, 3, 3, is>& matrix, const AABB<T, is>& aabb)
{
    return AABB<T, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is> operator * (const AABB<T, is>& aabb, const Matrix<T, 3, 3, is>& matrix)
{
    return AABB<T, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is> operator * (const Matrix<T, 4, 4, is>& matrix, const AABB<T, is>& aabb)
{
    return AABB<T, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is> operator * (const AABB<T, is>& aabb, const Matrix<T, 4, 4, is>& matrix)
{
    return AABB<T, is>(aabb) *= matrix;
}

/** Make minimum size AABB containing two aabbs */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is> merge(const AABB<T, is>& a, const AABB<T, is>& b)
{
    return AABB<T, is>( min(a.minVec, b.minVec), max(a.maxVec, b.maxVec) );
}

/** get nearest vector to the aabb form point */
template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 3, 1, is> get_nearest_direction(const AABB<T, is>& aabb, const Matrix<T, 3, 1, is>& vec)
{
    Matrix<T, 3, 1, is> direction;
    for(int i = 0; i<3; ++i) 
    {
        if (vec[i] > minVec[i])
        {
            if (vec[i] < maxVec[i]) {
                direction[i] = 0;
            }
            else {
                direction[i] = vec[i] - maxVec[i];
            }
        }
        else {
            direction[i] = minVec[i] - vec[i];
        }
    }

    return direction;
}


/** get nearest vector to the aabb form point */
template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 4, 1, is> get_nearest_direction(const AABB<T, is>& aabb, const Matrix<T, 4, 1, is>& vec)
{
    return Matrix<T, 4, 1, is>(get_nearest_direction( xyz(vec) ), 0);
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_AABB_H
