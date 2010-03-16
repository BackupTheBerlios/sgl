#ifndef SIMPLE_GL_GL_UNIFORM_H
#define SIMPLE_GL_GL_UNIFORM_H

#include "GLProgram.h"
#include "GLTexture.h"

namespace sgl {

template<typename Interface>
class GLUniformBase :
    public ReferencedImpl<Interface>
{
friend class GLProgram;
public:
    GLUniformBase( GLProgram*         _program,
                   const std::string& _name,
                   GLuint             _glIndex,
                   GLuint             _glLocation ) :
       program(_program),
       name(_name),
       glIndex(_glIndex),
       glLocation(_glLocation)
    {}

    // Override abstract uniform
    Program*    SGL_DLLCALL MasterProgram() const { return program; }
    const char* SGL_DLLCALL Name() const          { return name.c_str(); }

protected:
    // master
    GLProgram*  program;
    std::string name;

    // gl
    GLuint      glIndex;
    GLuint      glLocation; // location of the uniform
};

/* GL typed uniform */
template<typename T>
class GLUniform :
    public GLUniformBase< Uniform<T> >
{
friend class GLProgram;
public:
    typedef GLUniformBase< Uniform<T> >              base_type;
    typedef GLUniform<T>                             this_type;

public:
    GLUniform( GLProgram*          program,
               const std::string&  name,
               GLuint              glIndex,
               GLuint              glLocation,
               size_t              _numValues ) :
        base_type(program, name, glIndex, glLocation),
        numValues(_numValues)
    {}

    // Override Uniform
    void            SGL_DLLCALL Set(const T& value);
    void            SGL_DLLCALL Set(const T*       values,
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
    typedef GLUniformBase< SamplerUniform<T> >  base_type;
    typedef GLSamplerUniform<T>                 this_type;

public:
    GLSamplerUniform( GLProgram*         program,
                      const std::string& name,
                      GLuint             glStage,
                      GLuint             glLocation ) :
        base_type(program, name, glStage, glLocation)
    {}

    void SGL_DLLCALL Set(unsigned int stage, const T* texture)
    {
        if (texture) {
            texture->Bind(stage);
        }
        glUniform1i(base_type::glLocation, stage);
    }

    unsigned int SGL_DLLCALL Value() const
    {
        int value;
        glGetUniformiv(base_type::program->glProgram, base_type::glLocation, &value);
        return value;
    }
};

} // namespace sgl

#endif // SIMPLE_GL_GL_UNIFORM_H
