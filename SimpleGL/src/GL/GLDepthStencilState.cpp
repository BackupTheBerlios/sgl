#include "GL/GLDepthStencilState.h"
#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

namespace {

    GLenum BIND_COMPARISON_FUNCTION[] =
    {
	    GL_NEVER,
	    GL_LESS,
	    GL_LEQUAL,
	    GL_GREATER,
	    GL_GEQUAL,
	    GL_EQUAL,
	    GL_NOTEQUAL,
	    GL_ALWAYS
    };

    GLenum BIND_OPERATION[] =
    {
	    GL_KEEP,
	    GL_ZERO,
	    GL_REPLACE,
	    GL_INCR,
	    GL_DECR,
	    GL_INCR_WRAP,
	    GL_DECR_WRAP,
	    GL_INVERT,
    };

} // anonymous namespace

namespace sgl {

#ifndef SIMPLE_GL_ES

GLDepthStencilStateDisplayLists::GLDepthStencilStateDisplayLists(GLDevice* device_, const DESC& desc_) :
    device(device_),
    desc(desc_)
{
    bindDisplayList = glGenLists(1);

    // generate setup list
    glNewList(bindDisplayList, GL_COMPILE);
    {
        if (desc.depthEnable)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(BIND_COMPARISON_FUNCTION[desc.depthFunc]);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
        glDepthMask(desc.depthWriteMask);

        if (desc.stencilEnable)
        {
		    glEnable(GL_STENCIL_TEST);
            glStencilMask(desc.stencilWriteMask);

            glStencilFuncSeparate(GL_FRONT, BIND_COMPARISON_FUNCTION[desc.frontFaceOp.stencilFunc], 0, desc.stencilReadMask);
            glStencilFuncSeparate(GL_BACK,  BIND_COMPARISON_FUNCTION[desc.backFaceOp.stencilFunc],  0, desc.stencilReadMask);

            glStencilOpSeparate( GL_FRONT, 
                                 BIND_OPERATION[desc.frontFaceOp.stencilFailOp], 
                                 BIND_OPERATION[desc.frontFaceOp.stencilDepthFailOp],
                                 BIND_OPERATION[desc.frontFaceOp.stencilPassOp] );
            glStencilOpSeparate( GL_BACK, 
                                 BIND_OPERATION[desc.backFaceOp.stencilFailOp], 
                                 BIND_OPERATION[desc.backFaceOp.stencilDepthFailOp],
                                 BIND_OPERATION[desc.backFaceOp.stencilPassOp] );
        }
        else {
            glDisable(GL_STENCIL_TEST);
        }
    }
    glEndList();
}

GLDepthStencilStateDisplayLists::~GLDepthStencilStateDisplayLists()
{
    assert(device->CurrentDepthStencilState() != this);
    if (device->Valid()) {
        glDeleteLists(bindDisplayList, 1);
    }
}

// Override State
void GLDepthStencilStateDisplayLists::Bind() const
{
    glCallList(bindDisplayList);
    device->SetDepthStencilState(this);
}

#endif // SIMPLE_GL_ES

GLDepthStencilStateSeparate::GLDepthStencilStateSeparate(GLDevice* device_, const DESC& desc_) :
    device(device_),
    desc(desc_)
{
}

GLDepthStencilStateSeparate::~GLDepthStencilStateSeparate()
{
    assert(device->CurrentDepthStencilState() != this);
}

// Override State
void GLDepthStencilStateSeparate::Bind() const
{
    if (desc.depthEnable)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(BIND_COMPARISON_FUNCTION[desc.depthFunc]);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    glDepthMask(desc.depthWriteMask);

    if (desc.stencilEnable)
    {
        glEnable(GL_STENCIL_TEST);
        glStencilMask(desc.stencilWriteMask);

        glStencilFuncSeparate(GL_FRONT, BIND_COMPARISON_FUNCTION[desc.frontFaceOp.stencilFunc], 0, desc.stencilReadMask);
        glStencilFuncSeparate(GL_BACK,  BIND_COMPARISON_FUNCTION[desc.backFaceOp.stencilFunc],  0, desc.stencilReadMask);

        glStencilOpSeparate( GL_FRONT,
                             BIND_OPERATION[desc.frontFaceOp.stencilFailOp],
                             BIND_OPERATION[desc.frontFaceOp.stencilDepthFailOp],
                             BIND_OPERATION[desc.frontFaceOp.stencilPassOp] );
        glStencilOpSeparate( GL_BACK,
                             BIND_OPERATION[desc.backFaceOp.stencilFailOp],
                             BIND_OPERATION[desc.backFaceOp.stencilDepthFailOp],
                             BIND_OPERATION[desc.backFaceOp.stencilPassOp] );
    }
    else {
        glDisable(GL_STENCIL_TEST);
    }

    device->SetDepthStencilState(this);
}

} // namespace sgl
