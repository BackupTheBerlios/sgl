#ifndef SIMPLE_GL_GL_FFP_UNIFORM_H
#define SIMPLE_GL_GL_FFP_UNIFORM_H

#include "GLUniform.h"

namespace sgl {

/* GL typed uniform */
template<typename T>
class GLFFPUniform :
    public ReferencedImpl< Uniform<T> >
{
public:
    typedef void (*set_function)(const T*, unsigned int);
    typedef void (*get_function)(T*, unsigned int);

public:
    GLFFPUniform( Program*              _program,
                  const std::string&    _name,
                  set_function          _setFunction,
                  get_function          _getFunction,
                  size_t                _numValues ) :
        program(_program),
        name(_name),
        numValues(_numValues),
        setFunction(_setFunction),
        getFunction(_getFunction)
    {}

    // Override abstract uniform
    Program*        SGL_DLLCALL MasterProgram() const   { return program; }
    const char*     SGL_DLLCALL Name() const            { return name.c_str(); }
    unsigned int    SGL_DLLCALL Size() const            { return numValues; }

    /* Bind uniform to the fixed pipeline */
    void SGL_DLLCALL Set(const T& value)
    {
        setFunction(&value, 1);
    }
    
    void SGL_DLLCALL Set( const T*       values,
                          unsigned int   count )
    {
        setFunction(values, count);
    }

    T SGL_DLLCALL Value() const
    {
        T value;
        getFunction(&value, 1);
        return value;
    }

    void SGL_DLLCALL QueryValues(T* values) const
    {
        getFunction(values, numValues); 
    }

private:
    // program
    Program*        program;
    std::string     name;
    size_t          numValues;

    // bind
    set_function    setFunction;
    get_function    getFunction;
};

// typedefs
typedef GLFFPUniform<float>            GLFFPUniformF;
typedef GLFFPUniform<math::Vector2f>   GLFFPUniform2F;
typedef GLFFPUniform<math::Vector3f>   GLFFPUniform3F;
typedef GLFFPUniform<math::Vector4f>   GLFFPUniform4F;

typedef GLFFPUniform<int>              GLFFPUniformI;
typedef GLFFPUniform<math::Vector2i>   GLFFPUniform2I;
typedef GLFFPUniform<math::Vector3i>   GLFFPUniform3I;
typedef GLFFPUniform<math::Vector4i>   GLFFPUniform4I;

typedef GLFFPUniform<bool>             GLFFPUniformB;
typedef GLFFPUniform<math::Vector2b>   GLFFPUniform2B;
typedef GLFFPUniform<math::Vector3b>   GLFFPUniform3B;
typedef GLFFPUniform<math::Vector4b>   GLFFPUniform4B;

typedef GLFFPUniform<math::Matrix2f>   GLFFPUniform2x2F;
typedef GLFFPUniform<math::Matrix3f>   GLFFPUniform3x3F;
typedef GLFFPUniform<math::Matrix4f>   GLFFPUniform4x4F;

} // namespace sgl

#endif // SIMPLE_GL_GL_FFP_UNIFORM_H
