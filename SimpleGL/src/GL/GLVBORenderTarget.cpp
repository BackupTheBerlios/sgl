#include "GL/GLVBORenderTarget.h"
#include "GL/GLVertexDeclaration.h"
#include "DeviceTraits.h"

using namespace sgl;

static const GLenum BIND_TYPES[] =
{
    0,
    GL_BITMAP,
    GL_BYTE,
	GL_UNSIGNED_BYTE,
    GL_SHORT,
	GL_UNSIGNED_SHORT,
    GL_INT,
    GL_UNSIGNED_INT,
	GL_FLOAT
};

GLVBORenderTarget::GLVBORenderTarget(Device* _device) :
    base_type(_device)
{
    vboTargets.resize( colorSurface.size() );
}

void SGL_DLLCALL GLVBORenderTarget::Unbind() const
{
    // copy data
    for(size_t i = 0; i<vboTargets.size(); ++i)
    {
        if (vboTargets[i] && colorSurface[i])
        {
            glReadBuffer( GL_COLOR_ATTACHMENT0_EXT + i );
            glBindBufferARB( GL_PIXEL_PACK_BUFFER_ARB, vboTargets[i]->GLBufferObject() );

            GLuint glFormat = BIND_GL_FORMAT_USAGE[ colorSurface[i]->Format() ];
            GLuint glType   = BIND_TYPES[ vboTargets[i]->VertexDecl()->Element(0).type ];
            glReadPixels( 0, 0, colorSurface[i]->Width(), colorSurface[i]->Height(), glFormat, glType, 0 );
        }
    }

    glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, 0);
    base_type::Unbind();
}

// Override VBORenderTarget
SGL_HRESULT GLVBORenderTarget::SetVBOAttachment(unsigned int mrtIndex, VertexBuffer* vbo)
{
    #ifndef SGL_NO_STATUS_CHECK
    if ( mrtIndex >= vboTargets.size() ) {
        return EInvalidCall("SetVBOAttachment failed. Invalid index of the vbo.");
    }
    #endif

    vboTargets[mrtIndex].reset( static_cast<GLVertexBuffer*>(vbo) );
	return SGL_OK;
}

VertexBuffer* SGL_DLLCALL GLVBORenderTarget::VBOAttachment(unsigned int mrtIndex) const
{
    if ( mrtIndex < colorSurface.size() ) {
        return vboTargets[mrtIndex].get();
    }

    return NULL;
}
