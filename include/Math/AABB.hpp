#ifndef SIMPLE_GL_MATH_AABB_H
#define SIMPLE_GL_MATH_AABB_H

#include "../Utility/Meta.h"
#include "Ray.hpp"
#include "MatrixFunctions.hpp"
#include "Utility.hpp"
#include <cmath>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

/** Axis aligned bounding box. */
template< typename ValueType, 
          int n,
          INSTRUCTION_SET is = DEFAULT_INSTRUCTION_SET>
class AABB
{
private:
    typedef sgl::enable_if_c<n == 2>*       enable_if_2;
    typedef sgl::enable_if_c<n == 3>*       enable_if_3;

public:
    typedef AABB<ValueType, n, is>          this_type;
    typedef ValueType                       value_type;
    typedef Matrix<value_type, n, 1, is>    vec_type;

public:
    inline AABB() {}
    inline AABB(const vec_type& _minVec, const vec_type& _maxVec) :
        minVec(_minVec),
        maxVec(_maxVec)
    {}

    inline AABB(ValueType   minx,
                ValueType   miny,
                ValueType   maxx,
                ValueType   maxy,
                enable_if_2 toggle = 0) :
        minVec(minx, miny),
        maxVec(maxx, maxy)
    {}

    inline AABB(ValueType   minx,
                ValueType   miny,
                ValueType   minz,
                ValueType   maxx,
                ValueType   maxy,
                ValueType   maxz,
                enable_if_3 toggle = 0) :
        minVec(minx, miny, minz),
        maxVec(maxx, maxy, maxz)
    {}

    /** Setup aabb using top right far corner and bottom left near corner */
    inline AABB& setup(const vec_type& _minVec, const vec_type& _maxVec)
    {
        minVec = _minVec;
        maxVec = _maxVec;

        return *this;
    }
    /** Extend aabb so point will be covered */
    inline AABB& extend(const vec_type& vec)
    {
        minVec = min(vec, minVec);
        maxVec = max(vec, maxVec);

        return *this;
    }

    /** Get size of the box */
    inline vec_type size() const { return maxVec - minVec; }

    /** Get center of the box */
    inline vec_type center() const { return (maxVec + minVec) / 2; }

public:
    vec_type minVec;
    vec_type maxVec;
};

typedef AABB<float, 3>  AABBf;
typedef AABB<double, 3> AABBd;

// operators

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, 3, is>& operator *= (AABB<T, 3, is>& aabb, const Matrix<T, 3, 3, is>& matrix)
{
    AABB<T, 3, is>::vec_type    aMin, aMax;
    float                       a, b;

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
inline AABB<T, 3, is>& operator *= (AABB<T, 3, is>& aabb, const Matrix<T, 4, 4, is>& matrix)
{
    AABB<T, 3, is>::vec_type    aMin, aMax;
    float                       a, b;

    // Copy box A into min and max array.
    aMin = aabb.minVec;
    aMax = aabb.maxVec;

    // Begin at translation
    AABB<T, 3, is>::vec_type trans = get_translation(matrix);
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
template<typename T, int n, INSTRUCTION_SET is>
inline bool operator == (const AABB<T, n, is>& lhs, const AABB<T, n, is>& rhs) 
{ 
    return lhs.minVec == rhs.minVec && lhs.maxVec == rhs.maxVec; 
}

template<typename T, int n, INSTRUCTION_SET is>
inline bool operator != (const AABB<T, n, is>& lhs, const AABB<T, n, is>& rhs) 
{ 
    return lhs.minVec != rhs.minVec || lhs.maxVec != rhs.maxVec; 
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, is> operator * (const Matrix<T, 3, 3, is>& matrix, const AABB<T, 3, is>& aabb)
{
    return AABB<T, 3, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, 3, is> operator * (const AABB<T, 3, is>& aabb, const Matrix<T, 3, 3, is>& matrix)
{
    return AABB<T, 3, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, 3, is> operator * (const Matrix<T, 4, 4, is>& matrix, const AABB<T, 3, is>& aabb)
{
    return AABB<T, 3, is>(aabb) *= matrix;
}

/** Transform aabb by matrix */
template<typename T, INSTRUCTION_SET is>
inline AABB<T, 3, is> operator * (const AABB<T, 3, is>& aabb, const Matrix<T, 4, 4, is>& matrix)
{
    return AABB<T, 3, is>(aabb) *= matrix;
}

/** Make minimum size AABB containing two aabbs */
template<typename T, int n, INSTRUCTION_SET is>
inline AABB<T, n, is> merge(const AABB<T, n, is>& a, const AABB<T, n, is>& b)
{
    return AABB<T, n, is>( min(a.minVec, b.minVec), max(a.maxVec, b.maxVec) );
}

/** get nearest vector to the aabb form point */
template<typename T, INSTRUCTION_SET is>
inline Matrix<T, 3, 1, is> get_nearest_direction(const AABB<T, 3, is>& aabb, const Matrix<T, 3, 1, is>& vec)
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
inline Matrix<T, 4, 1, is> get_nearest_direction(const AABB<T, 3, is>& aabb, const Matrix<T, 4, 1, is>& vec)
{
    return Matrix<T, 4, 1, is>(get_nearest_direction( xyz(vec) ), 0);
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_AABB_H
