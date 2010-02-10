#ifndef SIMPLE_GL_UTILITY_H
#define SIMPLE_GL_UTILITY_H

#include "GLCommon.h"

namespace sgl {

class GLTypeTraits 
{
public:
    /** Get size of the gl type.
     * @param - could be one of the following: 
     * GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_INT,
	 * GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4, GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3,
	 * GL_BOOL_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_FLOAT_MAT2x3,
	 * GL_FLOAT_MAT2x4, GL_FLOAT_MAT3x2, GL_FLOAT_MAT3x4, GL_FLOAT_MAT4x2, GL_FLOAT_MAT4x3
     * @param return size of the value of specified type. 0 if value type is invalid.
     */ 
    static inline unsigned int GetSize(GLenum type)
    {
        switch(type)
        {
        case GL_BOOL:
            return sizeof(bool);

        case GL_FLOAT:
            return sizeof(float);

        case GL_INT:
            return sizeof(int);

        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
            return (type - GL_FLOAT_VEC2 + 2) * sizeof(float);

        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
            return (type - GL_INT_VEC2 + 2) * sizeof(int);

        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
            return (type - GL_BOOL_VEC2 + 2) * sizeof(bool);

        case GL_FLOAT_MAT2:
            return sizeof(math::Matrix2f);

        case GL_FLOAT_MAT3:
            return sizeof(math::Matrix3f);

        case GL_FLOAT_MAT4:
            return sizeof(math::Matrix4f);

        case GL_FLOAT_MAT2x3:
        case GL_FLOAT_MAT3x2:
            return 6 * sizeof(float);

        case GL_FLOAT_MAT2x4:
        case GL_FLOAT_MAT4x2:
            return 8 * sizeof(float);

        case GL_FLOAT_MAT3x4:
        case GL_FLOAT_MAT4x3:
            return 12 * sizeof(float);
        }

        return 0;
    }
};

} // namesapce sgl

#endif // SIMPLE_GL_UTILITY_H
