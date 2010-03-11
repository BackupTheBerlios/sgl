#ifndef SIMPLE_GL_MATH_KDOP_H
#define SIMPLE_GL_MATH_KDOP_H

#include "Plane.hpp"

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

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

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_KDOP_H
