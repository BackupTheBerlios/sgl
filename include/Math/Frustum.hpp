#ifndef SIMPLE_GL_MATH_FRUSTUM_H
#define SIMPLE_GL_MATH_FRUSTUM_H

#include "Plane.hpp"

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

template<typename ValueType, INSTRUCTION_SET is = DEFAULT_INSTRUCTION_SET>
class Frustum
#ifdef SIMPLE_GL_USE_SSE
    : public sgl::Aligned16
#endif
{
public:
    /// clip plane indices
    enum CLIP_PLANE
    {
        CP_LEFT    =   0,   
        CP_RIGHT   =   1,
        CP_TOP     =   2,
        CP_BOTTOM  =   3,
        CP_NEAR    =   4,
        CP_FAR     =   5,
        NUM_PLANES
    };

public:
    typedef Frustum<ValueType, is>          this_type;
    typedef ValueType                       value_type;

    typedef Matrix<value_type, 3, 1, is>    vec3_type;
    typedef Matrix<value_type, 4, 1, is>    vec4_type;
    typedef Matrix<value_type, 3, 3, is>    mat3_type;
    typedef Matrix<value_type, 4, 4, is>    mat4_type;

    typedef Plane<value_type>               plane_type;
    typedef AABB<value_type, is>            aabb_type;

private:
    // FIXME: Use something except define. Using static constant arise error in MSVS
    #define _0 static_cast<ValueType>(0)
    #define _1 static_cast<ValueType>(1)

public:
    Frustum() {}
    Frustum( const plane_type& plane0,
             const plane_type& plane1,
             const plane_type& plane2,
             const plane_type& plane3,
             const plane_type& plane4,
             const plane_type& plane5 ) :
    {
        planes[0] = plane0;
        planes[1] = plane1;
        planes[2] = plane2;
        planes[3] = plane3;
        planes[4] = plane4;
        planes[5] = plane5;
    }

    explicit Frustum(const mat4_type& clip)
    {
        setup(clip);
    }

    /** Setup frustum using model view projection matrix */
    Frustum& setup(const mat4_type& clip)
    {
        planes[CP_LEFT]  = plane_type( clip(12) + clip(0),
                                       clip(13) + clip(1),
                                       clip(14) + clip(2),
                                       clip(15) + clip(3) );

        planes[CP_RIGHT]  = plane_type( clip(12) - clip(0),
                                        clip(13) - clip(1),
                                        clip(14) - clip(2),
                                        clip(15) - clip(3) );

        planes[CP_TOP]    = plane_type( clip(12) - clip(4),
                                        clip(13) - clip(5),
                                        clip(14) - clip(6),
                                        clip(15) - clip(7) );

        planes[CP_BOTTOM] = plane_type( clip(12) + clip(4),
                                        clip(13) + clip(5),
                                        clip(14) + clip(6),
                                        clip(15) + clip(7) );

        planes[CP_NEAR]   = plane_type( clip(8),
                                        clip(9),
                                        clip(10),
                                        clip(11) );

        planes[CP_FAR]    = plane_type( clip(12) - clip(8),
                                        clip(13) - clip(9),
                                        clip(14) - clip(10),
                                        clip(15) - clip(11) );
        return *this;
    }

#undef _0
#undef _1

public:
    plane_type planes[NUM_PLANES];
};

typedef Frustum<float>  Frustumf;
typedef Frustum<double> Frustumd;

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_FRUSTUM_H
