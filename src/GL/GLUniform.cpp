#include "GL/GLUniform.h"

// uniforms
namespace sgl {

using namespace math;

#define DEFINE_UNIFORM(TYPE, CAST_TYPE, setFunction, getFunction)\
    template<>\
    void GLUniform<TYPE>::Set(const TYPE& value)\
    {\
        setFunction(glLocation, 1, (CAST_TYPE*)&value);\
    }\
    template<>\
    void GLUniform<TYPE>::Set(const TYPE*   values,\
                              unsigned int  count)\
    {\
        setFunction(glLocation, count, (CAST_TYPE*)values);\
    }\
    template<>\
    TYPE GLUniform<TYPE>::Value() const\
    {\
        TYPE v;\
        getFunction(glProgram, glLocation, (CAST_TYPE*)&v);\
        return v;\
    }\
    template<>\
    void GLUniform<TYPE>::QueryValues(TYPE* values) const\
    {\
        for(size_t i = 0; i<numValues; ++i)\
            getFunction(glProgram, glLocation, (CAST_TYPE*)&values[i]);\
    }

    DEFINE_UNIFORM(int,      int,   glUniform1iv, glGetUniformiv)
    DEFINE_UNIFORM(Vector2i, int,   glUniform2iv, glGetUniformiv)
    DEFINE_UNIFORM(Vector3i, int,   glUniform3iv, glGetUniformiv)
    DEFINE_UNIFORM(Vector4i, int,   glUniform4iv, glGetUniformiv)

    DEFINE_UNIFORM(float,    float, glUniform1fv, glGetUniformfv)
    DEFINE_UNIFORM(Vector2f, float, glUniform2fv, glGetUniformfv)
    DEFINE_UNIFORM(Vector3f, float, glUniform3fv, glGetUniformfv)
    DEFINE_UNIFORM(Vector4f, float, glUniform4fv, glGetUniformfv)
#undef DEFINE_UNIFORM

#define DEFINE_MATRIX_UNIFORM(TYPE, CAST_TYPE, setFunction, getFunction)\
    template<>\
    void GLUniform<TYPE>::Set(const TYPE& value)\
    {\
        setFunction(glLocation, 1, GL_TRUE, (CAST_TYPE*)&value);\
    }\
    template<>\
    void GLUniform<TYPE>::Set(const TYPE*   values,\
                              unsigned int  count)\
    {\
        setFunction(glLocation, count, GL_TRUE, (CAST_TYPE*)values);\
    }\
    template<>\
    TYPE GLUniform<TYPE>::Value() const\
    {\
        TYPE v;\
        getFunction(glProgram, glLocation, (CAST_TYPE*)&v);\
        return v;\
    }\
    template<>\
    void GLUniform<TYPE>::QueryValues(TYPE* values) const\
    {\
        for(size_t i = 0; i<numValues; ++i)\
            getFunction(glProgram, glLocation, (CAST_TYPE*)&values[i]);\
    }

    DEFINE_MATRIX_UNIFORM(Matrix2f, float, glUniformMatrix2fv, glGetUniformfv)
    DEFINE_MATRIX_UNIFORM(Matrix3f, float, glUniformMatrix3fv, glGetUniformfv)
    DEFINE_MATRIX_UNIFORM(Matrix4f, float, glUniformMatrix4fv, glGetUniformfv)

#undef DEFINE_MATRIX_UNIFORM

} // namespace sgl
