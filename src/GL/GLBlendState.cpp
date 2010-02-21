#include "GL/GLBlendState.h"

using namespace sgl;

namespace {

    GLenum BIND_BLEND_OPERATION[] =
    {
        GL_FUNC_ADD,
        GL_FUNC_SUBTRACT,
        GL_FUNC_REVERSE_SUBTRACT,
        GL_MIN,
        GL_MAX
    };

    GLenum BIND_BLEND_FUNCTION[] =
    {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA,
        GL_SRC_ALPHA_SATURATE
    };

} // anonymous namespace

GLBlendState::GLBlendState( sgl::Device*    _device, 
                            const DESC&     _desc ) :
    device(_device),
    desc(_desc)
{
    bindDisplayList = glGenLists(1);

    // generate setup list
    glNewList(bindDisplayList, GL_COMPILE);
    {
        if (desc.blendEnable)
        {
            glEnable(GL_BLEND);
            if (desc.srcBlend != desc.srcBlendAlpha || desc.destBlend != desc.destBlendAlpha)
            {
                glBlendFuncSeparate( BIND_BLEND_FUNCTION[desc.srcBlend], 
                                     BIND_BLEND_FUNCTION[desc.destBlend],
                                     BIND_BLEND_FUNCTION[desc.srcBlendAlpha],
                                     BIND_BLEND_FUNCTION[desc.destBlendAlpha] );
            }
            else 
            {
                glBlendFunc(BIND_BLEND_FUNCTION[desc.srcBlend], BIND_BLEND_FUNCTION[desc.destBlend]);
            }

            if (desc.blendOp != desc.blendOpAlpha) {
                glBlendEquationSeparate(BIND_BLEND_OPERATION[desc.blendOp], BIND_BLEND_OPERATION[desc.blendOpAlpha]);
            }
        }
        else {
            glDisable(GL_BLEND);
        }
    }
    glEndList();
}

GLBlendState::~GLBlendState()
{
    glDeleteLists(bindDisplayList, 1);
}

void GLBlendState::Bind() const
{
    glCallList(bindDisplayList);
    static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetBlendState(this);
}
