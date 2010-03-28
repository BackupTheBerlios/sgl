#ifndef SIMPLE_GL_MATH_KDOP_H
#define SIMPLE_GL_MATH_KDOP_H

#include "Plane.hpp"

SGL_BEGIN_MATH_NAMESPACE

template<typename           ValueType, 
         int                Dimension,
         int                NumPlanes>
class KDop
{
public:
    typedef KDop<ValueType, Dimension, NumPlanes>   this_type;
    typedef ValueType                               value_type;
    typedef Matrix<value_type, Dimension, 1>        vec_type;
    typedef Plane<value_type, Dimension>            plane_type;

public:
    plane_type planes[NumPlanes];
};

SGL_END_MATH_NAMESPACE

#endif // SIMPLE_GL_MATH_KDOP_H
