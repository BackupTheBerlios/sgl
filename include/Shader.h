#ifndef SIMPLE_GL_SHADER_H
#define SIMPLE_GL_SHADER_H

#include "Resource.h"

namespace sgl {

/** Shader interface */
class Shader :
    public Resource
{
public:
    /// Type of the shader
    enum TYPE
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY
    };

    /// Shader description
    struct DESC
    {
        TYPE            type;
        const char*     source;
    };

public:
    /** Get type of the shader. */
    virtual TYPE SGL_DLLCALL Type() const = 0;

    /** Get shader source. */
    virtual const char* SGL_DLLCALL Source() const = 0;

    /** Get shader compilation log. */
    virtual const char* SGL_DLLCALL CompilationLog() const = 0;

    virtual ~Shader() {}
};

} // namespace sgl

#endif // SIMPLE_GL_SHADER_H
