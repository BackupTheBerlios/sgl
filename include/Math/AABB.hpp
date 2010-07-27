#ifndef SIMPLE_GL_MATH_AABB_H
#define SIMPLE_GL_MATH_AABB_H

#include "MatrixFunctions.hpp"

SGL_BEGIN_MATH_NAMESPACE

/** Axis aligned bounding box. */
template<typename   ValueType,
         int        Dimension>
class AABB
{
public:
    typedef AABB<ValueType, Dimension>          this_type;
    typedef ValueType                           value_type;
    typedef Matrix<value_type, Dimension, 1>    vec_type;

public:
    inline AABB() {}
    inline AABB(const vec_type& _minVec, const vec_type& _maxVec) :
        minVec(_minVec),
        maxVec(_maxVec)
    {}

    inline AABB(ValueType   minx,
                ValueType   miny,
                ValueType   maxx,
                ValueType   maxy) :
        minVec(minx, miny),
        maxVec(maxx, maxy)
    {}

    inline AABB(ValueType   minx,
                ValueType   miny,
                ValueType   minz,
                ValueType   maxx,
                ValueType   maxy,
                ValueType   maxz) :
        minVec(minx, miny, minz),
        maxVec(maxx, maxy, maxz)
    {}

    /** Extend aabb so point will be covered */
    inline AABB& extend(const vec_type& vec)
    {
        minVec = min(vec, minVec);
        maxVec = max(vec, maxVec);

        return *this;
    }

    /** Make minimum size aabb */
    inline AABB& reset_min()
    {
        for (int i = 0; i<Dimension; ++i)
        {
            minVec[i] =  std::numeric_limits<value_type>::max();
            maxVec[i] = -std::numeric_limits<value_type>::max();
        }

        return *this;
    }

    /** Make maximum size aabb */
    inline AABB& reset_max()
    {
        for (int i = 0; i<Dimension; ++i)
        {
            minVec[i] = -std::numeric_limits<value_type>::max();
            maxVec[i] =  std::numeric_limits<value_type>::max();
        }

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

typedef AABB<float, 2>  AABB2f;
typedef AABB<double, 2> AABB2d;

typedef AABB<float, 3>  AABB3f;
typedef AABB<double, 3> AABB3d;

typedef AABB<float, 4>  AABB4f;
typedef AABB<double, 4> AABB4d;

typedef AABB<float, 3>  AABBf;
typedef AABB<double, 3> AABBd;

// functions
template<typename T, int n>
inline AABB<T, n> extend(const AABB<T, n>& aabb, const Matrix<T, n, 1>& vec)
{
    return AABB<T, n>( min(vec, aabb.minVec), max(vec, aabb.maxVec) );
}

template<typename T, int n>
inline Matrix<T, n, 1> size(const AABB<T, n>& aabb)
{
    return aabb.maxVec - aabb.minVec;
}

template<typename T, int n>
inline Matrix<T, n, 1> center(const AABB<T, n>& aabb)
{
    return (aabb.maxVec + aabb.minVec) / 2;
}

// operators
template<typename T>
inline AABB<T, 3>& operator *= (AABB<T, 3>& aabb, const Matrix<T, 3, 3>& matrix)
{
    typename AABB<T, 3>::vec_type   aMin, aMax;
    float                           a, b;

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

template<typename T>
inline AABB<T, 3>& operator *= (AABB<T, 3>& aabb, const Matrix<T, 4, 4>& matrix)
{
    typename AABB<T, 3>::vec_type   aMin, aMax;
    float                           a, b;

    // Copy box A into min and max array.
    aMin = aabb.minVec;
    aMax = aabb.maxVec;

    // Begin at translation
    typename AABB<T, 3>::vec_type trans = get_translation(matrix);
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
template<typename T, int n>
inline bool operator == (const AABB<T, n>& lhs, const AABB<T, n>& rhs)
{
    return lhs.minVec == rhs.minVec && lhs.maxVec == rhs.maxVec;
}

template<typename T, int n>
inline bool operator != (const AABB<T, n>& lhs, const AABB<T, n>& rhs)
{
    return lhs.minVec != rhs.minVec || lhs.maxVec != rhs.maxVec;
}

/** Transform aabb by matrix */
template<typename T>
inline AABB<T, 3> operator * (const Matrix<T, 3, 3>& matrix, const AABB<T, 3>& aabb)
{
    AABB<T, 3> tmp(aabb);
    return tmp *= matrix;
}

/** Transform aabb by matrix */
template<typename T>
inline AABB<T, 3> operator * (const AABB<T, 3>& aabb, const Matrix<T, 3, 3>& matrix)
{
    AABB<T, 3> tmp(aabb);
    return tmp *= matrix;
}

/** Transform aabb by matrix */
template<typename T>
inline AABB<T, 3> operator * (const Matrix<T, 4, 4>& matrix, const AABB<T, 3>& aabb)
{
    AABB<T, 3> tmp(aabb);
    return tmp *= matrix;
}

/** Transform aabb by matrix */
template<typename T>
inline AABB<T, 3> operator * (const AABB<T, 3>& aabb, const Matrix<T, 4, 4>& matrix)
{
    AABB<T, 3> tmp(aabb);
    return tmp *= matrix;
}

/** Make minimum size AABB containing two aabbs */
template<typename T, int n>
inline AABB<T, n> merge(const AABB<T, n>& a, const AABB<T, n>& b)
{
    return AABB<T, n>( min(a.minVec, b.minVec), max(a.maxVec, b.maxVec) );
}

/** get nearest vector to the aabb form point */
template<typename T>
inline Matrix<T, 3, 1> get_nearest_direction(const AABB<T, 3>& aabb, const Matrix<T, 3, 1>& vec)
{
    Matrix<T, 3, 1> direction;
    for(int i = 0; i<3; ++i)
    {
        if (vec[i] > aabb.minVec[i])
        {
            if (vec[i] < aabb.maxVec[i]) {
                direction[i] = 0;
            }
            else {
                direction[i] = vec[i] - aabb.maxVec[i];
            }
        }
        else {
            direction[i] = aabb.minVec[i] - vec[i];
        }
    }

    return direction;
}


/** get nearest vector to the aabb form point */
template<typename T>
inline Matrix<T, 4, 1> get_nearest_direction(const AABB<T, 3>& aabb, const Matrix<T, 4, 1>& vec)
{
    return Matrix<T, 4, 1>(get_nearest_direction( xyz(vec) ), 0);
}

SGL_END_MATH_NAMESPACE

#endif // SIMPLE_GL_MATH_AABB_H
