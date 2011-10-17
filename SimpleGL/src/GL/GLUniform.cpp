#include "GL/GLUniform.h"

// uniforms
namespace sgl {

using namespace math;

#define DEFINE_UNIFORM(UTYPE, CTYPE, CAST_TYPE, setFunction, getFunction)\
    template<>\
    AbstractUniform::TYPE GLUniform<CTYPE>::Type() const\
    {\
        return UTYPE;\
    }\
    template<>\
    void GLUniform<CTYPE>::Set(const CTYPE& value)\
    {\
        assert( device->CurrentProgram() == program );\
        setFunction(glLocation, 1, (CAST_TYPE*)&value);\
    }\
    template<>\
    void GLUniform<CTYPE>::Set(const CTYPE*  values,\
                               unsigned int  count)\
    {\
        assert( device->CurrentProgram() == program );\
        setFunction(glLocation, count, (CAST_TYPE*)values);\
    }\
    template<>\
    CTYPE GLUniform<CTYPE>::Value() const\
    {\
        assert( device->CurrentProgram() == program );\
        CTYPE v;\
        getFunction(glProgram, glLocation, (CAST_TYPE*)&v);\
        return v;\
    }\
    template<>\
    void GLUniform<CTYPE>::QueryValues(CTYPE* values) const\
    {\
        assert( device->CurrentProgram() == program );\
        for(size_t i = 0; i<numValues; ++i)\
            getFunction(glProgram, glLocation, (CAST_TYPE*)&values[i]);\
    }

    DEFINE_UNIFORM(INT,   int,      int,   glUniform1iv, glGetUniformiv)
    DEFINE_UNIFORM(VEC2I, Vector2i, int,   glUniform2iv, glGetUniformiv)
    DEFINE_UNIFORM(VEC3I, Vector3i, int,   glUniform3iv, glGetUniformiv)
    DEFINE_UNIFORM(VEC4I, Vector4i, int,   glUniform4iv, glGetUniformiv)

    DEFINE_UNIFORM(FLOAT, float,    float, glUniform1fv, glGetUniformfv)
    DEFINE_UNIFORM(VEC2F, Vector2f, float, glUniform2fv, glGetUniformfv)
    DEFINE_UNIFORM(VEC3F, Vector3f, float, glUniform3fv, glGetUniformfv)
    DEFINE_UNIFORM(VEC4F, Vector4f, float, glUniform4fv, glGetUniformfv)
#undef DEFINE_UNIFORM

#ifdef SIMPLE_GL_ES
#	define TRANSPOSE_MATRIX GL_FALSE
#else
#	define TRANSPOSE_MATRIX GL_TRUE
#endif

#define DEFINE_MATRIX_UNIFORM(UTYPE, CTYPE, CAST_TYPE, setFunction, getFunction)\
    template<>\
    AbstractUniform::TYPE GLUniform<CTYPE>::Type() const\
    {\
        return UTYPE;\
    }\
    template<>\
    void GLUniform<CTYPE>::Set(const CTYPE& value)\
    {\
        assert( device->CurrentProgram() == program );\
        setFunction(glLocation, 1, TRANSPOSE_MATRIX, (CAST_TYPE*)&value);\
    }\
    template<>\
    void GLUniform<CTYPE>::Set(const CTYPE* values,\
                               unsigned int count)\
    {\
        assert( device->CurrentProgram() == program );\
        setFunction(glLocation, count, TRANSPOSE_MATRIX, (CAST_TYPE*)values);\
    }\
    template<>\
    CTYPE GLUniform<CTYPE>::Value() const\
    {\
        assert( device->CurrentProgram() == program );\
        CTYPE v;\
        getFunction(glProgram, glLocation, (CAST_TYPE*)&v);\
        return v;\
    }\
    template<>\
    void GLUniform<CTYPE>::QueryValues(CTYPE* values) const\
    {\
        for(size_t i = 0; i<numValues; ++i)\
            getFunction(glProgram, glLocation, (CAST_TYPE*)&values[i]);\
    }

    DEFINE_MATRIX_UNIFORM(MAT2x2F,  Matrix2x2f, float, glUniformMatrix2fv,      glGetUniformfv)
#ifndef SIMPLE_GL_ES
    DEFINE_MATRIX_UNIFORM(MAT2x3F,  Matrix2x3f, float, glUniformMatrix2x3fv,    glGetUniformfv)
    DEFINE_MATRIX_UNIFORM(MAT2x4F,  Matrix2x4f, float, glUniformMatrix2x4fv,    glGetUniformfv)
    DEFINE_MATRIX_UNIFORM(MAT3x2F,  Matrix3x2f, float, glUniformMatrix3x2fv,    glGetUniformfv)
#endif
    DEFINE_MATRIX_UNIFORM(MAT3x3F,  Matrix3x3f, float, glUniformMatrix3fv,      glGetUniformfv)
#ifndef SIMPLE_GL_ES
    DEFINE_MATRIX_UNIFORM(MAT3x4F,  Matrix3x4f, float, glUniformMatrix3x4fv,    glGetUniformfv)
    DEFINE_MATRIX_UNIFORM(MAT4x2F,  Matrix4x2f, float, glUniformMatrix4x2fv,    glGetUniformfv)
    DEFINE_MATRIX_UNIFORM(MAT4x3F,  Matrix4x3f, float, glUniformMatrix4x3fv,    glGetUniformfv)
#endif
    DEFINE_MATRIX_UNIFORM(MAT4x4F,  Matrix4x4f, float, glUniformMatrix4fv,      glGetUniformfv)

#undef DEFINE_MATRIX_UNIFORM

template<>
AbstractUniform::TYPE GLSamplerUniform<Texture1D>::Type() const
{
    return AbstractUniform::SAMPLER_1D;
}

template<>
AbstractUniform::TYPE GLSamplerUniform<Texture2D>::Type() const
{
    return AbstractUniform::SAMPLER_2D;
}

template<>
AbstractUniform::TYPE GLSamplerUniform<Texture3D>::Type() const
{
    return AbstractUniform::SAMPLER_3D;
}

template<>
AbstractUniform::TYPE GLSamplerUniform<TextureCube>::Type() const
{
    return AbstractUniform::SAMPLER_CUBE;
}

} // namespace sgl
