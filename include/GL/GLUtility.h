#ifndef SIMPLE_GL_UTILITY_H
#define SIMPLE_GL_UTILITY_H

#include "GLCommon.h"

namespace sgl {

class GLTypeTraits 
{
public:
    /** Get size of the gl type.
     * @param - opengl type param.
     * @param return size of the value of specified type. 0 if provided type is invalid
     */ 
    static inline unsigned int Size(GLenum type)
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

    /** Convert OpenGL type to sgl.
     * @param - opengl type param.
     * @return corresponding sgl type or __NUMBER_OF_SCALAR_TYPES__ if can't map type.
     */ 
    static SCALAR_TYPE Type(GLenum type)
    {
        switch(type)
        {
        case GL_BITMAP:
            return BIT;

        case GL_BOOL:
            return BOOL;

        case GL_BYTE:
            return BYTE;

        case GL_UNSIGNED_BYTE:
            return UBYTE;

        case GL_SHORT:
            return SHORT;

        case GL_UNSIGNED_SHORT:
            return USHORT;

        case GL_INT:
            return INT;

        case GL_UNSIGNED_INT:
            return UINT;

        case GL_FLOAT:
            return FLOAT;

        case GL_DOUBLE:
            return DOUBLE;

        default:
            return __NUMBER_OF_SCALAR_TYPES__;
        }

        return __NUMBER_OF_SCALAR_TYPES__;
    }
};

} // namesapce sgl

#endif // SIMPLE_GL_UTILITY_H
