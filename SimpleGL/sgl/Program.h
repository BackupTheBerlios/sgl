#ifndef SIMPLE_GL_PROGRAM_H
#define SIMPLE_GL_PROGRAM_H

#include "Shader.h"
#include "Uniform.h"
#include "VertexBuffer.h"

namespace sgl {

/// Primitive type
enum PRIMITIVE_TYPE
{
    POINTS,				
    LINES,
    LINE_STRIP,
    LINE_LOOP,			
    TRIANGLES,
    TRIANGLE_STRIP,		
    TRIANGLE_FAN,
    /// QUADS,				/// Deprecated since OpenGL 3.1 and not supported in OpenGL 3.2, OpenGL ES
    /// QUAD_STRIP,			/// Deprecated since OpenGL 3.1 and not supported in OpenGL 3.2, OpenGL ES
    /// POLYGON,			/// Deprecated since OpenGL 3.1 and not supported in OpenGL 3.2, OpenGL ES
    __NUMBER_OF_PRIMITIVE_TYPES__
};

/** Shader program interface */
class Program :
	public Resource
{
public:
    struct ATTRIBUTE
    {
        unsigned            index;
        unsigned            size;
        const char*         name;
        sgl::SCALAR_TYPE    type;
    };

public:
    /** Attach shader to the program. Program becames dirty.
     * @return Result of the operation. Could be EInvalidCall if the shader is invalid
     */
    virtual SGL_HRESULT SGL_DLLCALL AddShader(Shader* shader) = 0;

    /** Detach shader from the program. Program becames dirty.
     * @param shader - shader for detaching.
     * @return true if shader was detached. false if not found.
     */
    virtual bool SGL_DLLCALL RemoveShader(Shader* shader) = 0;

    /** Check wether program wants to be recreated. */
    virtual bool SGL_DLLCALL IsDirty() const = 0;

    /** Clear program data */
    virtual void SGL_DLLCALL Clear() = 0;

    /** Recreate the program. If succeeded program becomes valid and can be used.
     * @param force recreation even if it is already initialized.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL Dirty(bool force = false) = 0;

    /** Get program compilation log */
    virtual const char* SGL_DLLCALL CompilationLog() const = 0;

    /** Setup program to the device.
     * @return result of the operation. Can be SGLERR_INVALID_CALL if program is dirty.
     */
    virtual SGL_HRESULT SGL_DLLCALL Bind() const = 0;

    /** Unbind program form the device. */
    virtual void SGL_DLLCALL Unbind() const = 0;

    /** Set maximum number of vertices generated by the geometry shader. Program becames dirty.
     * @param maxNumVertices - maximum number of vertices generated by geometry shader
     * @return result of the operation. Can set SGLERR_UNSUPPORTED if number of vertices is
     * greater than supported by the device, or if the geometry shaders are no supported.
     */
    virtual void SGL_DLLCALL SetGeometryNumVerticesOut(unsigned int maxNumVertices) = 0;

    /** Set input primitives type for the geometry shader. Program becames dirty.
     * @param inputType - type of the input primitives.
     * @return result of the operation. Can set SGLERR_UNSUPPORTED if the geometry shaders
     * are no supported.
     */
    virtual void SGL_DLLCALL SetGeometryInputType(PRIMITIVE_TYPE inputType) = 0;

    /** Set output primitives type for the geometry shader. Program becames dirty.
     * @param outputType - type of the output primitives.
     * @return result of the operation. Can set SGLERR_UNSUPPORTED if the geometry shaders
     * are no supported.
     */
    virtual void SGL_DLLCALL SetGeometryOutputType(PRIMITIVE_TYPE outputType) = 0;

    /** Get maximum number of geometry shader output vertices. Default is 0. */
    virtual unsigned int   SGL_DLLCALL GeometryNumVerticesOut() const = 0;

    /** Get geometry shader input primitives type. Default is POINTS. */
    virtual PRIMITIVE_TYPE SGL_DLLCALL GeometryInputType() const = 0;

    /** Get geometry shader output primitives type. Default is POINTS. */
    virtual PRIMITIVE_TYPE SGL_DLLCALL GeometryOutputType() const = 0;

    /** Bind named attribute to the attribute index. 
     * @param name - name of the attribute to bind.
     * @param index - index of the attribute in the vertex declaration. NOTE: Some attributes are reserved!
     * @return Any of the Dirty result. SGL_INVALID_CALL if the name doesn't correspond any vertex attribute.
     */
    virtual SGL_HRESULT SGL_DLLCALL BindAttributeLocation(const char* name, unsigned int index) = 0;

    /** Get named attribute location
     * @param name - name of the attribute.
     * @return location of the attribute or -1 if not found.
     */
    virtual int         SGL_DLLCALL AttributeLocation(const char* name) const = 0;

    /** Get number of attributes used by the program. */
    virtual unsigned    SGL_DLLCALL NumAttributes() const = 0;

    /** Get definition of the i'th program attribute. */
    virtual ATTRIBUTE   SGL_DLLCALL Attribute(unsigned index) const = 0;

    /** Get abstract uniform */
    virtual AbstractUniform*        SGL_DLLCALL GetUniform(const char* name) const = 0;

    /** Create int uniform */
    virtual UniformI*               SGL_DLLCALL GetUniformI(const char* name) const = 0;

    /** Create Vector2i uniform */
    virtual Uniform2I*              SGL_DLLCALL GetUniform2I(const char* name) const = 0;

    /** Create Vector3i uniform */
    virtual Uniform3I*              SGL_DLLCALL GetUniform3I(const char* name) const = 0;

    /** Create Vector4i uniform */
    virtual Uniform4I*              SGL_DLLCALL GetUniform4I(const char* name) const = 0;

    /** Create float uniform */
    virtual UniformF*               SGL_DLLCALL GetUniformF(const char* name) const = 0;

    /** Create Vector2f uniform */
    virtual Uniform2F*              SGL_DLLCALL GetUniform2F(const char* name) const = 0;

    /** Create Vector3f uniform */
    virtual Uniform3F*              SGL_DLLCALL GetUniform3F(const char* name) const = 0;

    /** Create Vector4f uniform */
    virtual Uniform4F*              SGL_DLLCALL GetUniform4F(const char* name) const = 0;

    /** Create Matrix2f uniform */
    virtual Uniform2x2F*            SGL_DLLCALL GetUniform2x2F(const char* name) const = 0;

    /** Create Matrix2x3f uniform */
    //virtual Uniform2x3F*            SGL_DLLCALL GetUniform3x3F(const char* name) = 0;

    /** Create Matrix2x4f uniform */
    //virtual Uniform2x4F*            SGL_DLLCALL GetUniform3x3F(const char* name) = 0;

    /** Create Matrix3x2f uniform */
    //virtual Uniform3x2F*            SGL_DLLCALL GetUniform3x3F(const char* name) = 0;

    /** Create Matrix3f uniform */
    virtual Uniform3x3F*            SGL_DLLCALL GetUniform3x3F(const char* name) const = 0;

    /** Create Matrix3f uniform */
    //virtual Uniform3x4F*            SGL_DLLCALL GetUniform3x3F(const char* name) = 0;

    /** Create Matrix4x3f uniform */
    //virtual Uniform4x3F*            SGL_DLLCALL GetUniform3x3F(const char* name) = 0;

    /** Create Matrix4f uniform */
    virtual Uniform4x4F*            SGL_DLLCALL GetUniform4x4F(const char* name) const = 0;

    /** Create Texture1D uniform */
    virtual SamplerUniform1D*       SGL_DLLCALL GetSamplerUniform1D(const char* name) const = 0;

    /** Create Texture2D uniform */
    virtual SamplerUniform2D*       SGL_DLLCALL GetSamplerUniform2D(const char* name) const = 0;

    /** Create Texture3D uniform */
    virtual SamplerUniform3D*       SGL_DLLCALL GetSamplerUniform3D(const char* name) const = 0;

    /** Create TextureCube uniform */
    virtual SamplerUniformCube*     SGL_DLLCALL GetSamplerUniformCube(const char* name) const = 0;

    /** Get uniform with specified value type */
    template<typename T>
    static inline Uniform<T>* SGL_DLLCALL GetUniform( Program*      program,
                                                      const char*   name );

    /** Get sampler uniform with specified value type */
    template<typename T>
    static inline SamplerUniform<T>* SGL_DLLCALL GetSamplerUniform( Program*       program,
                                                                    const char*    name );


    virtual ~Program() {}
};

template<>
inline UniformF* SGL_DLLCALL Program::GetUniform<float>( Program*     program,
                                                         const char*  name )
{
    return program->GetUniformF(name);
}

template<>
inline Uniform2F* SGL_DLLCALL Program::GetUniform<math::Vector2f>( Program*     program,
                                                                   const char*  name )
{
    return program->GetUniform2F(name);
}

template<>
inline Uniform3F* SGL_DLLCALL Program::GetUniform<math::Vector3f>( Program*     program,
                                                                    const char*  name )
{
    return program->GetUniform3F(name);
}

template<>
inline Uniform4F* SGL_DLLCALL Program::GetUniform<math::Vector4f>( Program*     program,
                                                                    const char*  name )
{
    return program->GetUniform4F(name);
}

template<>
inline Uniform2x2F* SGL_DLLCALL Program::GetUniform<math::Matrix2f>( Program*     program,
                                                                     const char*  name )
{
    return program->GetUniform2x2F(name);
}

template<>
inline Uniform3x3F* SGL_DLLCALL Program::GetUniform<math::Matrix3f>( Program*     program,
                                                                     const char*  name )
{
    return program->GetUniform3x3F(name);
}

template<>
inline Uniform4x4F* SGL_DLLCALL Program::GetUniform<math::Matrix4f>( Program*     program,
                                                                     const char*  name )
{
    return program->GetUniform4x4F(name);
}

template<>
inline UniformI* SGL_DLLCALL Program::GetUniform<int>( Program*     program,
                                                       const char*  name )
{
    return program->GetUniformI(name);
}

template<>
inline Uniform2I* SGL_DLLCALL Program::GetUniform<math::Vector2i>( Program*     program,
                                                                   const char*  name)
{
    return program->GetUniform2I(name);
}

template<>
inline Uniform3I* SGL_DLLCALL Program::GetUniform<math::Vector3i>( Program*     program,
                                                                   const char*  name)
{
    return program->GetUniform3I(name);
}

template<>
inline Uniform4I* SGL_DLLCALL Program::GetUniform<math::Vector4i>( Program*     program,
                                                                   const char*  name )
{
    return program->GetUniform4I(name);
}

// sampler uniforms
template<>
inline SamplerUniform1D* SGL_DLLCALL Program::GetSamplerUniform<Texture1D>( Program*     program,
                                                                            const char*  name )
{
    return program->GetSamplerUniform1D(name);
}

template<>
inline SamplerUniform2D* SGL_DLLCALL Program::GetSamplerUniform<Texture2D>( Program*     program,
                                                                            const char*  name )
{
    return program->GetSamplerUniform2D(name);
}

template<>
inline SamplerUniform3D* SGL_DLLCALL Program::GetSamplerUniform<Texture3D>( Program*     program,
                                                                            const char*  name )
{
    return program->GetSamplerUniform3D(name);
}

template<>
inline SamplerUniformCube* SGL_DLLCALL Program::GetSamplerUniform<TextureCube>( Program*     program,
                                                                                const char*  name )
{
    return program->GetSamplerUniformCube(name);
}

} // namepace sgl

#endif // SIMPLE_GL_PROGRAM_H
