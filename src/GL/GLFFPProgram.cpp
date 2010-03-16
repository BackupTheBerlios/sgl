#include "GL/GLDevice.h"
#include "GL/GLFFPProgram.h"
#include "GL/GLFFPUniform.h"
#include "Math/MatrixFunctions.hpp"
#include <algorithm>
#include <functional>

using namespace sgl;

namespace {

    void set_modelview_matrix(const math::Matrix4f* matrices, unsigned int)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadTransposeMatrixf( matrices[0].data() );
    }

    void set_modelview_matrix_direct(const math::Matrix4f* matrices, unsigned int)
    {
        glMatrixLoadTransposefEXT( GL_MODELVIEW, matrices[0].data() );
    }

    void get_modelview_matrix(math::Matrix4f* matrices, unsigned int)
    {
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)matrices);
        matrices[0] = math::transpose(matrices[0]);
    }

    void set_projection_matrix(const math::Matrix4f* matrices, unsigned int)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadTransposeMatrixf( matrices[0].data() );
    }

    void set_projection_matrix_direct(const math::Matrix4f* matrices, unsigned int)
    {
        glMatrixLoadTransposefEXT( GL_PROJECTION, matrices[0].data() );
    }

    void get_projection_matrix(math::Matrix4f* matrices, unsigned int)
    {
        glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)matrices);
        matrices[0] = math::transpose(matrices[0]);
    }

    void set_texture_matrix(const math::Matrix4f* matrices, unsigned int quantity)
    {
        glMatrixMode(GL_TEXTURE);
        for (unsigned int i = 0; i<quantity; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glLoadTransposeMatrixf( matrices[i].data() );
        }
    }

    void set_texture_matrix_direct(const math::Matrix4f* matrices, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glMatrixLoadTransposefEXT( GL_TEXTURE0 + i, matrices[i].data() );
        }
    }

    void get_texture_matrix(math::Matrix4f* matrices, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glGetFloatv(GL_TEXTURE_MATRIX, (GLfloat*)&matrices[i]);
            matrices[i] = math::transpose(matrices[i]);
        }
    }

    void set_lighting_toggle(const int* toggles, unsigned int)
    {
        if (*toggles) {
            glEnable(GL_LIGHTING);
        }
        else {
            glDisable(GL_LIGHTING);
        }
    }

    void get_lighting_toggle(int* toggles, unsigned int)
    {
        toggles[0] = glIsEnabled(GL_LIGHTING);
    }

    void set_light_toggle(const int* toggles, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i)
        {
            if (*toggles) {
                glEnable(GL_LIGHT0 + i);
            }
            else {
                glDisable(GL_LIGHT0 + i);
            }
        }
    }

    void get_light_toggle(int* toggles, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i)
        {
            toggles[i] = glIsEnabled(GL_LIGHT0 + i);
        }
    }

    void set_light_position(const math::Vector4f* positions, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glLightfv(GL_LIGHT0 + i, GL_POSITION, &positions[i].x);
        }
    }

    void get_light_position(math::Vector4f* positions, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glGetLightfv(GL_LIGHT0 + i, GL_POSITION, &positions[i].x);
        }
    }

    void set_light_direction(const math::Vector4f* directions, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, &directions[i].x);
        }
    }

    void get_light_direction(math::Vector4f* directions, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glGetLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, &directions[i].x);
        }
    }

    void set_light_ambient(const math::Vector4f* ambients, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, &ambients[i].x);
        }
    }

    void get_light_ambient(math::Vector4f* ambients, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glGetLightfv(GL_LIGHT0 + i, GL_AMBIENT, &ambients[i].x);
        }
    }

    void set_light_diffuse(const math::Vector4f* diffuses, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &diffuses[i].x);
        }
    }

    void get_light_diffuse(math::Vector4f* diffuses, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glGetLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &diffuses[i].x);
        }
    }

    void set_light_specular(const math::Vector4f* speculars, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &speculars[i].x);
        }
    }

    void get_light_specular(math::Vector4f* speculars, unsigned int quantity)
    {
        for (unsigned int i = 0; i<quantity; ++i) {
            glGetLightfv(GL_LIGHT0 + i, GL_SPECULAR, &speculars[i].x);
        }

    }

    void set_ambient(const math::Vector4f* ambient, unsigned int)
    {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &ambient[0].x);
    }

    void get_ambient(math::Vector4f* ambient, unsigned int)
    {
        glGetFloatv(GL_LIGHT_MODEL_AMBIENT, &ambient[0].x);
    }

    void set_material_ambient(const math::Vector4f* ambient, unsigned int)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &ambient[0].x);
    }

    void get_material_ambient(math::Vector4f* ambient, unsigned int)
    {
        glGetMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &ambient[0].x);
    }

    void set_material_diffuse(const math::Vector4f* diffuse, unsigned int)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &diffuse[0].x);
    }

    void get_material_diffuse(math::Vector4f* diffuse, unsigned int)
    {
        glGetMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &diffuse[0].x);
    }

    void set_material_specular(const math::Vector4f* specular, unsigned int)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &specular[0].x);
    }

    void get_material_specular(math::Vector4f* specular, unsigned int)
    {
        glGetMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &specular[0].x);
    }

    void set_material_emission(const math::Vector4f* emission, unsigned int)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &emission[0].x);
    }

    void get_material_emission(math::Vector4f* emission, unsigned int)
    {
        glGetMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &emission[0].x);
    }

    void set_material_shininess(const float* shininess, unsigned int)
    {
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, *shininess);
    }

    void get_material_shininess(float* shininess, unsigned int)
    {
        glGetMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    }

} // anonymous namespace

GLFFPProgram::GLFFPProgram(Device* _device) :
    device(_device)
{
    using namespace math;

    // create uniforms
    bool   directStateAccess = (glewIsSupported("GL_EXT_direct_state_access") == GL_TRUE);
    size_t offset            = 0;

    // modelview
    size_t uniformSize = sizeof(Matrix4f);
    if (directStateAccess)
    {
        modelViewMatrixUniform.reset( new GLFFPUniform4x4F( this,
                                                            "ModelViewMatrix",
                                                            set_modelview_matrix_direct,
                                                            get_modelview_matrix,
                                                            uniformSize ) );
    }
    else {
        modelViewMatrixUniform.reset( new GLFFPUniform4x4F( this,
                                                            "ModelViewMatrix",
                                                            set_modelview_matrix,
                                                            get_modelview_matrix,
                                                            uniformSize ) );
    }
    offset += uniformSize;

    // projection
    uniformSize = sizeof(Matrix4f);
    if (directStateAccess)
    {
        projectionMatrixUniform.reset( new GLFFPUniform4x4F( this,
                                                             "ProjectionMatrix",
                                                             set_projection_matrix_direct,
                                                             get_projection_matrix,
                                                             uniformSize ) );
    }
    else {
        projectionMatrixUniform.reset( new GLFFPUniform4x4F( this,
                                                             "ProjectionMatrix",
                                                             set_projection_matrix,
                                                             get_projection_matrix,
                                                             uniformSize ) );
    }
    offset += uniformSize;

    // texture
    uniformSize = sizeof(Matrix4f) * Device::NUM_TEXTURE_STAGES;
    if (directStateAccess)
    {
        textureMatrixUniform.reset( new GLFFPUniform4x4F( this,
                                                          "TextureMatrix",
                                                          set_texture_matrix_direct,
                                                          get_texture_matrix,
                                                          uniformSize ) );
    }
    else {
        textureMatrixUniform.reset( new GLFFPUniform4x4F( this,
                                                          "TextureMatrix",
                                                          set_texture_matrix,
                                                          get_texture_matrix,
                                                          uniformSize ) );
    }
    offset += uniformSize;

    // light position
    uniformSize = sizeof(Vector4f) * Device::NUM_FFP_LIGHTS;
    lightDirectionUniform.reset( new GLFFPUniform4F( this,
                                                     "LightPosition",
                                                     set_light_position,
                                                     get_light_position,
                                                     uniformSize ) );
    offset += uniformSize;

    // light direction
    uniformSize = sizeof(Vector4f) * Device::NUM_FFP_LIGHTS;
    lightPositionUniform.reset( new GLFFPUniform4F( this,
                                                    "LightDirection",
                                                    set_light_direction,
                                                    get_light_direction,
                                                    uniformSize ) );
    offset += uniformSize;

    // light ambient
    uniformSize = sizeof(Vector4f) * Device::NUM_FFP_LIGHTS;
    lightAmbientUniform.reset( new GLFFPUniform4F( this,
                                                   "LightAmbient",
                                                   set_light_ambient,
                                                   get_light_ambient,
                                                   uniformSize ) );
    offset += uniformSize;

    // light diffuse
    uniformSize = sizeof(Vector4f) * Device::NUM_FFP_LIGHTS;
    lightDiffuseUniform.reset( new GLFFPUniform4F( this,
                                                   "LightDiffuse",
                                                   set_light_diffuse,
                                                   get_light_diffuse,
                                                   uniformSize ) );
    offset += uniformSize;

    // light specular
    uniformSize = sizeof(Vector4f) * Device::NUM_FFP_LIGHTS;
    lightSpecularUniform.reset( new GLFFPUniform4F( this,
                                                    "LightSpecular",
                                                    set_light_specular,
                                                    get_light_specular,
                                                    uniformSize ) );
    offset += uniformSize;

    // ambient
    uniformSize = sizeof(Vector4f);
    ambientUniform.reset( new GLFFPUniform4F( this,
                                              "Ambient",
                                              set_ambient,
                                              get_ambient,
                                              uniformSize ) );
    offset += uniformSize;

    // material ambient
    uniformSize = sizeof(Vector4f);
    materialAmbientUniform.reset( new GLFFPUniform4F( this,
                                                      "MaterialAmbient",
                                                      set_material_ambient,
                                                      get_material_ambient,
                                                      uniformSize ) );
    offset += uniformSize;

    // material diffuse
    uniformSize = sizeof(Vector4f);
    materialDiffuseUniform.reset( new GLFFPUniform4F( this,
                                                      "MaterialDiffuse",
                                                      set_material_diffuse,
                                                      get_material_diffuse,
                                                      uniformSize ) );
    offset += uniformSize;

    // material specular
    uniformSize = sizeof(Vector4f);
    materialSpecularUniform.reset( new GLFFPUniform4F( this,
                                                       "MaterialSpecular",
                                                       set_material_specular,
                                                       get_material_specular,
                                                       uniformSize ) );
    offset += uniformSize;

    // material emission
    uniformSize = sizeof(Vector4f);
    materialEmissionUniform.reset( new GLFFPUniform4F( this,
                                                       "MaterialEmission",
                                                       set_material_emission,
                                                       get_material_emission,
                                                       uniformSize ) );
    offset += uniformSize;

    // material shininess
    uniformSize = sizeof(float);
    materialShininessUniform.reset( new GLFFPUniformF( this,
                                                       "MaterialShininess",
                                                       set_material_shininess,
                                                       get_material_shininess,
                                                       uniformSize ) );
    offset += uniformSize;

    // lighting toggle
    uniformSize = sizeof(bool);
    lightingToggleUniform.reset( new GLFFPUniformI( this,
                                                    "LightingToggle",
                                                    set_lighting_toggle,
                                                    get_lighting_toggle,
                                                    uniformSize ) );
    offset += uniformSize;

    // light toggles
    uniformSize = sizeof(bool) * Device::NUM_FFP_LIGHTS;
    lightToggleUniform.reset( new GLFFPUniformI( this,
                                                 "LightToggle",
                                                 set_light_toggle,
                                                 get_light_toggle,
                                                 uniformSize ) );
    offset += uniformSize;
}

GLFFPProgram::~GLFFPProgram()
{
    // Do nothing. Dies with device
}

SGL_HRESULT GLFFPProgram::BindAttributeLocation(const char* /*name*/, unsigned /*index*/)
{
    return EInvalidCall("Can't bind attribute location for ffp program.");
}

SGL_HRESULT GLFFPProgram::Bind() const
{
    glUseProgram(0);
    static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetProgram(this);

	return SGL_OK;
}

void SGL_DLLCALL GLFFPProgram::Unbind() const
{
    // Actually we can't unbind ffp program
}
