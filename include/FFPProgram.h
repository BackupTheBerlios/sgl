#ifndef SIMPLE_GL_FFP_PROGRAM_H
#define SIMPLE_GL_FFP_PROGRAM_H

#include "Program.h"

namespace sgl {

/** Program that uses(or emulates) fixed function pipeline */
class FFPProgram :
	public Program
{
public:
    /** Get uniform to setup single 4x4 model view matrix. */
    virtual Uniform4x4F* SGL_DLLCALL GetModelViewMatrixUniform() const = 0;

    /** Get uniform to setup single 4x4 projection matrix. */
    virtual Uniform4x4F* SGL_DLLCALL GetProjectionMatrixUniform() const = 0;

    /** Get uniform to setup array of texture matrices for each sampler. */
    virtual Uniform4x4F* SGL_DLLCALL GetTextureMatrixUniform() const = 0;

    /** Get uniform to setup lighting toggle. */
    virtual UniformI* SGL_DLLCALL GetLightingToggleUniform() const = 0;

    /** Get uniform to setup array of light toggles. */
    virtual UniformI* SGL_DLLCALL GetLightToggleUniform() const = 0;

    /** Get uniform to setup array of light positions. */
    virtual Uniform4F* SGL_DLLCALL GetLightPositionUniform() const = 0;

    /** Get uniform to setup array of light directions. */
    virtual Uniform4F* SGL_DLLCALL GetLightDirectionUniform() const = 0;

    /** Get uniform to setup array of light ambient colors. */
    virtual Uniform4F* SGL_DLLCALL GetLightAmbientUniform() const = 0;

    /** Get uniform to setup array of light diffuse colors. */
    virtual Uniform4F* SGL_DLLCALL GetLightDiffuseUniform() const = 0;

    /** Get uniform to setup array of light specular colors. */
    virtual Uniform4F* SGL_DLLCALL GetLightSpecularUniform() const = 0;

    /** Get uniform to setup ambient lighting color. */
    virtual Uniform4F* SGL_DLLCALL GetAmbientUniform() const = 0;

    /** Get uniform to setup material ambient color. */
    virtual Uniform4F* SGL_DLLCALL GetMaterialAmbientUniform() const = 0;

    /** Get uniform to setup material diffuse color. */
    virtual Uniform4F* SGL_DLLCALL GetMaterialDiffuseUniform() const = 0;

    /** Get uniform to setup material emission color. */
    virtual Uniform4F* SGL_DLLCALL GetMaterialEmissionUniform() const = 0;

    /** Get uniform to setup material specular color. */
    virtual Uniform4F* SGL_DLLCALL GetMaterialSpecularUniform() const = 0;

    /** Get uniform to setup material shininess. */
    virtual UniformF* SGL_DLLCALL GetMaterialShininessUniform() const = 0;

    virtual ~FFPProgram() {}
};

} // namepace sgl

#endif // SIMPLE_GL_FFP_PROGRAM_H
