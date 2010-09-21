#ifndef SIMPLE_GL_GL_FFP_PROGRAM_H
#define SIMPLE_GL_GL_FFP_PROGRAM_H

#include "GLShader.h"

namespace sgl {

class GLFFPProgram :
    public ResourceImpl<FFPProgram>
{
public:
    GLFFPProgram(GLDevice* device);

    // shaders
    SGL_HRESULT         SGL_DLLCALL AddShader(Shader* /*shader*/)    { return EInvalidCall("You can't add shaders to the FFP program"); }
    bool                SGL_DLLCALL RemoveShader(Shader* /*shader*/) { return false; }

    // Work
    bool                SGL_DLLCALL IsDirty() const         { return false; }
    SGL_HRESULT         SGL_DLLCALL Dirty(bool /*force*/)   { return SGL_OK; }
    void                SGL_DLLCALL Clear()                 {}
    SGL_HRESULT         SGL_DLLCALL Bind() const;
    void                SGL_DLLCALL Unbind() const;

    // Info
    const char*         SGL_DLLCALL CompilationLog() const      { return 0; }

    // Geometry shaders
    void                SGL_DLLCALL SetGeometryNumVerticesOut(unsigned int /*maxNumVertices*/)  {}
    void                SGL_DLLCALL SetGeometryInputType(PRIMITIVE_TYPE /*inputType*/)          {}
    void                SGL_DLLCALL SetGeometryOutputType(PRIMITIVE_TYPE /*outputType*/)        {}

    unsigned int        SGL_DLLCALL GeometryNumVerticesOut() const  { return 0; }
    PRIMITIVE_TYPE      SGL_DLLCALL GeometryInputType() const       { return POINTS; }
    PRIMITIVE_TYPE      SGL_DLLCALL GeometryOutputType() const      { return POINTS; }

    /* Attributes */
    SGL_HRESULT         SGL_DLLCALL BindAttributeLocation(const char* name, unsigned index);
    int                 SGL_DLLCALL AttributeLocation(const char* /*name*/) const { return -1; }
    unsigned            SGL_DLLCALL NumAttributes() const                         { return 0; }
    ATTRIBUTE           SGL_DLLCALL Attribute(unsigned /*index*/) const           { sglSetError(SGLERR_INVALID_CALL, "FFP program doesn't have generic attributes"); return ATTRIBUTE(); }

    /* Standart uniforms */
    AbstractUniform*    SGL_DLLCALL GetUniform(const char* name) const      { return 0; }

    Uniform4x4F*        SGL_DLLCALL GetModelViewMatrixUniform() const       { return modelViewMatrixUniform.get(); }
    Uniform4x4F*        SGL_DLLCALL GetProjectionMatrixUniform() const      { return projectionMatrixUniform.get(); }
    Uniform4x4F*        SGL_DLLCALL GetTextureMatrixUniform() const         { return textureMatrixUniform.get(); }

    UniformI*           SGL_DLLCALL GetLightingToggleUniform() const        { return lightingToggleUniform.get(); }
    UniformI*           SGL_DLLCALL GetLightToggleUniform() const           { return lightToggleUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetLightPositionUniform() const         { return lightPositionUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetLightDirectionUniform() const        { return lightDirectionUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetLightAmbientUniform() const          { return lightAmbientUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetLightDiffuseUniform() const          { return lightDiffuseUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetLightSpecularUniform() const         { return lightSpecularUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetAmbientUniform() const               { return ambientUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetMaterialAmbientUniform() const       { return materialAmbientUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetMaterialDiffuseUniform() const       { return materialDiffuseUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetMaterialEmissionUniform() const      { return materialEmissionUniform.get(); }
    Uniform4F*          SGL_DLLCALL GetMaterialSpecularUniform() const      { return materialSpecularUniform.get(); }
    UniformF*           SGL_DLLCALL GetMaterialShininessUniform() const     { return materialShininessUniform.get(); }

    /* Create int uniform */
    UniformI*           SGL_DLLCALL GetUniformI(const char* /*name*/) const           { return 0; }
    Uniform2I*          SGL_DLLCALL GetUniform2I(const char* /*name*/) const          { return 0; }
    Uniform3I*          SGL_DLLCALL GetUniform3I(const char* /*name*/) const          { return 0; }
    Uniform4I*          SGL_DLLCALL GetUniform4I(const char* /*name*/) const          { return 0; }

    /* Create float uniform */
    UniformF*           SGL_DLLCALL GetUniformF(const char* /*name*/) const           { return 0; }
    Uniform2F*          SGL_DLLCALL GetUniform2F(const char* /*name*/) const          { return 0; }
    Uniform3F*          SGL_DLLCALL GetUniform3F(const char* /*name*/) const          { return 0; }
    Uniform4F*          SGL_DLLCALL GetUniform4F(const char* /*name*/) const          { return 0; }

    /* Create matrix uniform */
    Uniform2x2F*        SGL_DLLCALL GetUniform2x2F(const char* /*name*/) const        { return 0; }
    Uniform3x3F*        SGL_DLLCALL GetUniform3x3F(const char* /*name*/) const        { return 0; }
    Uniform4x4F*        SGL_DLLCALL GetUniform4x4F(const char* /*name*/) const        { return 0; }

    /* Create texture uniforms */
    SamplerUniform1D*   SGL_DLLCALL GetSamplerUniform1D(const char* /*name*/) const   { return 0; }
    SamplerUniform2D*   SGL_DLLCALL GetSamplerUniform2D(const char* /*name*/) const   { return 0; }
    SamplerUniform3D*   SGL_DLLCALL GetSamplerUniform3D(const char* /*name*/) const   { return 0; }
    SamplerUniformCube* SGL_DLLCALL GetSamplerUniformCube(const char* /*name*/) const { return 0; }

private:
    GLDevice*    device;

    // standart uniforms
    scoped_ptr<Uniform4x4F> modelViewMatrixUniform;
    scoped_ptr<Uniform4x4F> projectionMatrixUniform;
    scoped_ptr<Uniform4x4F> textureMatrixUniform;
    scoped_ptr<Uniform4F>   lightPositionUniform;
    scoped_ptr<Uniform4F>   lightDirectionUniform;
    scoped_ptr<Uniform4F>   lightAmbientUniform;
    scoped_ptr<Uniform4F>   lightDiffuseUniform;
    scoped_ptr<Uniform4F>   lightSpecularUniform;
    scoped_ptr<Uniform4F>   ambientUniform;
    scoped_ptr<Uniform4F>   materialAmbientUniform;
    scoped_ptr<Uniform4F>   materialDiffuseUniform;
    scoped_ptr<Uniform4F>   materialSpecularUniform;
    scoped_ptr<Uniform4F>   materialEmissionUniform;
    scoped_ptr<UniformF>    materialShininessUniform;
    scoped_ptr<UniformI>    lightingToggleUniform;
    scoped_ptr<UniformI>    lightToggleUniform;
};

} // namespaec sgl

#endif // SIMPLE_GL_GL_FFP_PROGRAM_H
