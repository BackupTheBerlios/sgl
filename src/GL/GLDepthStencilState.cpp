#include "GL/GLDepthStencilState.h"

using namespace sgl;

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

GLDepthStencilState::GLDepthStencilState( sgl::Device*  _device, 
                                          const DESC&   _desc ) :
    device(_device),
    desc(_desc)
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

GLDepthStencilState::~GLDepthStencilState()
{
    glDeleteLists(bindDisplayList, 1);
}

void GLDepthStencilState::Bind() const
{
    glCallList(bindDisplayList);
    static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetDepthStencilState(this);
}
