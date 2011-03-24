#include "GL/GLBlendState.h"
#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

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

namespace sgl {

GLBlendStateDisplayLists::GLBlendStateDisplayLists(GLDevice*              device_, 
												  const BlendState::DESC& desc_) :
    device(device_),
    desc(desc_)
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
            
GLBlendStateDisplayLists::~GLBlendStateDisplayLists()
{
    assert(device->CurrentBlendState() != this);
    glDeleteLists(bindDisplayList, 1);
}

void GLBlendStateDisplayLists::Bind() const
{
    glCallList(bindDisplayList);
    device->SetBlendState(this);
}

GLBlendStateSeparate::GLBlendStateSeparate(GLDevice*               device_, 
										   const BlendState::DESC& desc_) :
    device(device_),
    desc(desc_)
{
}

GLBlendStateSeparate::~GLBlendStateSeparate()
{
    assert(device->CurrentBlendState() != this);
}

void GLBlendStateSeparate::Bind() const
{
    if (desc.blendEnable)
    {
        glEnable(GL_BLEND);
        glBlendFuncSeparate( BIND_BLEND_FUNCTION[desc.srcBlend],
                             BIND_BLEND_FUNCTION[desc.destBlend],
                             BIND_BLEND_FUNCTION[desc.srcBlendAlpha],
                             BIND_BLEND_FUNCTION[desc.destBlendAlpha] );
        glBlendEquationSeparate( BIND_BLEND_OPERATION[desc.blendOp],
                                 BIND_BLEND_OPERATION[desc.blendOpAlpha] );
    }
    else {
        glDisable(GL_BLEND);
    }

    device->SetBlendState(this);
}

GLBlendStateDefault::GLBlendStateDefault(GLDevice*   device_, 
										 const BlendState::DESC&    desc_) :
    device(device_),
    desc(desc_)
{
    glSrcBlend  = BIND_BLEND_FUNCTION[desc.srcBlend];
    glDestBlend = BIND_BLEND_FUNCTION[desc.destBlend];
    glBlendOp   = BIND_BLEND_OPERATION[desc.blendOp];
}

GLBlendStateDefault::~GLBlendStateDefault()
{
    assert(device->CurrentBlendState() != this);
}

void GLBlendStateDefault::Bind() const
{
    if (desc.blendEnable)
    {
        glEnable(GL_BLEND);
        glBlendFunc(glSrcBlend, glDestBlend);
        glBlendEquation(glBlendOp);
    }
    else {
        glDisable(GL_BLEND);
    }

    device->SetBlendState(this);
}

template<DEVICE_VERSION DeviceVersion>
sgl::BlendState* sglCreateBlendState(GLDevice* device, const BlendState::DESC& desc)
{
    if ( desc.destBlend != desc.destBlendAlpha 
         || desc.srcBlend != desc.srcBlendAlpha
         || desc.blendOp != desc.blendOpAlpha )
    {
        if (!GL_ARB_draw_buffers_blend) {
            throw gl_error("Separate blending is not supported", SGLERR_UNSUPPORTED);
        }

        if ( device_traits<DeviceVersion>::support_display_lists() ) {
            return new GLBlendStateDisplayLists(device, desc);
        }
        else {
            return new GLBlendStateSeparate(device, desc);
        }
    }
    else {
        return new GLBlendStateDefault(device, desc);
    }
}

} // namespace sgl