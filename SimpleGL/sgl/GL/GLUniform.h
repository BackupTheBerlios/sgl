#ifndef SIMPLE_GL_GL_UNIFORM_H
#define SIMPLE_GL_GL_UNIFORM_H

#include "GLTexture.h"

namespace sgl {

template<typename Interface>
class GLUniformBase :
    public ReferencedImpl<Interface>
{
public:
    GLUniformBase( Device*              _device,
                   const Program*       _program,
                   const std::string&   _name,
                   GLuint               _glProgram,
                   GLuint               _glIndex,
                   GLuint               _glLocation ) :
       device(_device),
       program(_program),
       name(_name),
       glProgram(_glProgram),
       glIndex(_glIndex),
       glLocation(_glLocation)
    {}

    // Override abstract uniform
    const Program*  SGL_DLLCALL MasterProgram() const { return program; }
    const char*     SGL_DLLCALL Name() const          { return name.c_str(); }

    /** Get OpenGL uniform index */
    GLuint SGL_DLLCALL Index() const { return glIndex; }

    /** Get OpenGL uniform location */
    GLuint SGL_DLLCALL Location() const { return glLocation; }

protected:
    // master
    Device*         device;
    const Program*  program;
    std::string     name;

    // gl
    GLuint          glProgram;
    GLuint          glIndex;
    GLuint          glLocation;
};

/* GL typed uniform */
template<typename T>
class GLUniform :
    public GLUniformBase< Uniform<T> >
{
public:
    typedef GLUniformBase< Uniform<T> >  base_type;

public:
    GLUniform( Device*              device,
               const Program*       program,
               const std::string&   name,
               GLuint               glProgram,
               GLuint               glIndex,
               GLuint               glLocation,
               size_t               _numValues ) :
        base_type(device, program, name, glProgram, glIndex, glLocation),
        numValues(_numValues)
    {}

    // Override Uniform
    AbstractUniform::TYPE   SGL_DLLCALL Type() const;
    void                    SGL_DLLCALL Set(const T& value);
    void                    SGL_DLLCALL Set(const T*       values,
                                            unsigned int   count);
    T               SGL_DLLCALL Value() const;
    unsigned int    SGL_DLLCALL Size() const { return numValues; }
    void            SGL_DLLCALL QueryValues(T* values) const;

private:
    size_t numValues;
};

/* GL sampler uniform */
template<typename T>
class GLSamplerUniform :
    public GLUniformBase< SamplerUniform<T> >
{
public:
    typedef GLUniformBase< SamplerUniform<T> >   base_type;

public:
    GLSamplerUniform( Device*               device,
                      const Program*        program,
                      const std::string&    name,
                      GLuint                glProgram,
                      GLuint                glStage,
                      GLuint                glLocation ) :
        base_type(device, program, name, glProgram, glStage, glLocation)
    {}

    AbstractUniform::TYPE SGL_DLLCALL Type() const;

    void SGL_DLLCALL Set(unsigned int stage, const T* texture)
    {
        assert( base_type::device->CurrentProgram() == base_type::program );
        if (texture) {
            texture->Bind(stage);
        }
        glUniform1i(base_type::glLocation, stage);
    }

    unsigned int SGL_DLLCALL Value() const
    {
        assert( base_type::device->CurrentProgram() == base_type::program );
        int value;
        glGetUniformiv(base_type::glProgram, base_type::glLocation, &value);
        return value;
    }
};

} // namespace sgl

#endif // SIMPLE_GL_GL_UNIFORM_H
