#include "GL/GLRasterizerState.h"

using namespace sgl;

namespace {

    GLenum BIND_CULL_MODE[] =
    {
        GL_NONE,
        GL_BACK,
        GL_FRONT,
	    GL_FRONT_AND_BACK
    };

    GLenum BIND_FILL_MODE[] =
    {
	    GL_FILL,
	    GL_LINE
    };

} // anonymous namespace

GLRasterizerState::GLRasterizerState( sgl::Device*  _device, 
                                      const DESC&   _desc ) :
    device(_device),
    desc(_desc),
    scissorRect(0, 0, 0, 0)
{
    bindDisplayList = glGenLists(1);

    // generate setup list
    glNewList(bindDisplayList, GL_COMPILE);
    {
        if (desc.cullMode != NONE)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(BIND_CULL_MODE[desc.cullMode]);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        glPolygonMode(GL_FRONT_AND_BACK, BIND_FILL_MODE[desc.fillMode]);
        if (desc.scissorEnable) 
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(scissorRect[0], scissorRect[1], scissorRect[2] - scissorRect[0], scissorRect[3] - scissorRect[1]);
        }
        else {
            glDisable(GL_SCISSOR_TEST);
        }
    }
    glEndList();
}

GLRasterizerState::~GLRasterizerState()
{
    glDeleteLists(bindDisplayList, 1);
}

void GLRasterizerState::SetScissorRectangle(const math::Vector4i& rect)
{
}

void GLRasterizerState::Bind() const
{
    glCallList(bindDisplayList);
    static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetRasterizerState(this);
}
