#ifndef SIMPLE_GL_MATH_FRUSTUM_H
#define SIMPLE_GL_MATH_FRUSTUM_H

#include "KDop.hpp"

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

typedef KDop<float, 3, 6>  Frustumf;
typedef KDop<double, 3, 6> Frustumd;

/** Setup frustum using model view projection matrix */
template<typename T>
KDop<T, 3, 6> make_frustum(const Matrix<T, 4, 4>& clip)
{
    typedef math::Plane<T, 3> plane_type;

    KDop<T, 3, 6> frustum;
    frustum.planes[0] = plane_type( clip(12) + clip(0),
                                    clip(13) + clip(1),
                                    clip(14) + clip(2),
                                    clip(15) + clip(3) );
    frustum.planes[1] = plane_type( clip(12) - clip(0),
                                    clip(13) - clip(1),
                                    clip(14) - clip(2),
                                    clip(15) - clip(3) );
    frustum.planes[2] = plane_type( clip(12) - clip(4),
                                    clip(13) - clip(5),
                                    clip(14) - clip(6),
                                    clip(15) - clip(7) );
    frustum.planes[3] = plane_type( clip(12) + clip(4),
                                    clip(13) + clip(5),
                                    clip(14) + clip(6),
                                    clip(15) + clip(7) );
    frustum.planes[4] = plane_type( clip(8),
                                    clip(9),
                                    clip(10),
                                    clip(11) );
    frustum.planes[5] = plane_type( clip(12) - clip(8),
                                    clip(13) - clip(9),
                                    clip(14) - clip(10),
                                    clip(15) - clip(11) );

    return frustum;
}

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_FRUSTUM_H
