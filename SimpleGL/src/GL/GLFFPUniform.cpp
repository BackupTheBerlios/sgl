#include "GL/GLFFPUniform.h"

// uniforms
namespace sgl {

using namespace math;

#define DEFINE_UNIFORM(UTYPE, CTYPE)\
    template<>\
    AbstractUniform::TYPE GLFFPUniform<CTYPE>::Type() const\
    {\
        return UTYPE;\
    }

    DEFINE_UNIFORM(AbstractUniform::INT,      int)
    DEFINE_UNIFORM(AbstractUniform::VEC2I,    Vector2i)
    DEFINE_UNIFORM(AbstractUniform::VEC3I,    Vector3i)
    DEFINE_UNIFORM(AbstractUniform::VEC4I,    Vector4i)

    DEFINE_UNIFORM(AbstractUniform::FLOAT,    float)
    DEFINE_UNIFORM(AbstractUniform::VEC2F,    Vector2f)
    DEFINE_UNIFORM(AbstractUniform::VEC3F,    Vector3f)
    DEFINE_UNIFORM(AbstractUniform::VEC4F,    Vector4f)

    DEFINE_UNIFORM(AbstractUniform::MAT2x2F,  Matrix2x2f)
#ifndef SIMPLE_GL_ES
    DEFINE_UNIFORM(AbstractUniform::MAT2x3F,  Matrix2x3f)
    DEFINE_UNIFORM(AbstractUniform::MAT2x4F,  Matrix2x4f)
    DEFINE_UNIFORM(AbstractUniform::MAT3x2F,  Matrix3x2f)
#endif
    DEFINE_UNIFORM(AbstractUniform::MAT3x3F,  Matrix3x3f)
#ifndef SIMPLE_GL_ES
    DEFINE_UNIFORM(AbstractUniform::MAT3x4F,  Matrix3x4f)
    DEFINE_UNIFORM(AbstractUniform::MAT4x2F,  Matrix4x2f)
    DEFINE_UNIFORM(AbstractUniform::MAT4x3F,  Matrix4x3f)
#endif
    DEFINE_UNIFORM(AbstractUniform::MAT4x4F,  Matrix4x4f)
#undef DEFINE_UNIFORM

} // namespace sgl
