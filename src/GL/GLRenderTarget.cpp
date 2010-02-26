#include "GL/GLCommon.h"
#include "GL/GLRenderTarget.h"
#include <algorithm>
#include <functional>

namespace {

    GLuint GuardedBind(GLuint fbo)
    {
        GLuint oldFBO;
	    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO);
	    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

        return oldFBO;
    }

    void GuardedUnbind(GLuint oldFBO)
    {
	    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldFBO);
    }

} // anonymous namespace

namespace sgl {

GLRenderTarget::GLRenderTarget(Device* _device) :
    device(_device),
    dirty(true),
    useDepthStencilRenderbuffer(false),
    fbo(0),
    dsRenderBuffer(0),
    readBuffer(GL_COLOR_ATTACHMENT0)
{
    attachments.resize(Device::NUM_TEXTURE_STAGES);

    glGenFramebuffersEXT(1, &fbo);
    glGenRenderbuffersEXT(1, &dsRenderBuffer);
}

GLRenderTarget::~GLRenderTarget()
{
    if (fbo) {
        glDeleteFramebuffersEXT(1, &fbo);
    }
    if (dsRenderBuffer) {
        glDeleteRenderbuffersEXT(1, &dsRenderBuffer);
    }
}

SGL_HRESULT GLRenderTarget::SetDepthStencil( bool             toggle,
                                             Texture::FORMAT  format, 
                                             unsigned int     samples )
{
#ifndef SGL_NO_STATUS_CHECK
    if ( !Texture::FORMAT_TRAITS[format].depth ) {
        return EInvalidCall("GLFBO::SetDepthStencil failed. Provided format is not depth format.");
    }
#endif

    dirty                            = true;
    useDepthStencilRenderbuffer      = toggle;
    dsRenderBufferAttachment.format  = format;
    dsRenderBufferAttachment.samples = samples;

    return SGL_OK;
}

SGL_HRESULT GLRenderTarget::SetColorAttachment( unsigned int    mrtIndex,
                                                Texture2D*      texture, 
                                                unsigned int    level )
{
#ifndef SGL_NO_STATUS_CHECK
    if ( mrtIndex >= attachments.size() ) {
        return EInvalidCall("GLFBO::SetColorAttachment failed. MRT index is too large.");
    }
#endif

	dirty                 = true;
	attachments[mrtIndex] = attachment(static_cast<GLTexture<Texture2D>*>(texture), level);

    return SGL_OK;
}

SGL_HRESULT GLRenderTarget::SetColorAttachment( unsigned int    mrtIndex,
                                                Texture3D*      texture, 
                                                unsigned int    level,
                                                unsigned int    layer )
{
#ifndef SGL_NO_STATUS_CHECK
    if ( mrtIndex >= attachments.size() ) {
        return EInvalidCall("GLFBO::SetColorAttachment failed. MRT index is too large.");
    }
#endif

	dirty                 = true;
	attachments[mrtIndex] = attachment(static_cast<GLTexture<Texture3D>*>(texture), level, layer);

    return SGL_OK;
}

SGL_HRESULT GLRenderTarget::SetDepthStencilAttachment( Texture2D*   depthStencilTexture,
                                                       unsigned int level )
{
#ifndef SGL_NO_STATUS_CHECK
    if ( !Texture::FORMAT_TRAITS[ depthStencilTexture->Format() ].depth ) {
        return EInvalidCall("GLFBO::SetDepthStencilAttachment failed. Provided format is not depth format.");
    }
#endif

    dirty        = true;
    dsAttachment = attachment(static_cast<GLTexture<Texture2D>*>(depthStencilTexture), level);

    return SGL_OK;
}

Texture* GLRenderTarget::ColorAttachment(unsigned int mrtIndex) const
{
    if ( mrtIndex < attachments.size() ) {
        return attachments[mrtIndex].texture.get();
    }

    return 0;
}

Texture* GLRenderTarget::DepthStencilAttachment() const
{
    return dsAttachment.texture.get();
}

bool GLRenderTarget::HaveDepthStencil() const
{
    return (dsAttachment.texture != 0) || useDepthStencilRenderbuffer;
}

bool GLRenderTarget::IsDirty() const
{
    return dirty;
}

SGL_HRESULT GLRenderTarget::Dirty(bool force)
{
    if (!dirty && !force) {
        return SGL_OK;
    }

    // bind
    GLenum error;
    GLuint oldFBO               = GuardedBind(fbo);
    size_t maxAttachmentWidth   = 0;
    size_t maxAttachmentHeight  = 0;
    bool   fillDrawBuffers      = drawBuffers.empty();
    for(size_t i = 0; i<attachments.size(); ++i)
    {
        const GLRenderTarget::attachment& attachment = attachments[i];
        if (attachment.glTarget)
        {
            if (fillDrawBuffers) {
                drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            }

            // get size
            maxAttachmentHeight = std::max(maxAttachmentHeight, attachment.width);
            maxAttachmentWidth  = std::max(maxAttachmentHeight, attachment.height);

            // attach color buffer
            switch (attachment.glTarget) 
            {
            case GL_TEXTURE_2D:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT,
                                           GL_COLOR_ATTACHMENT0 + i,
                                           attachment.glTarget,
                                           attachment.glTexture,
                                           attachment.level );
                break;

            case GL_TEXTURE_3D:
                glFramebufferTexture3DEXT( GL_FRAMEBUFFER_EXT,
                                           GL_COLOR_ATTACHMENT0 + i,
                                           attachment.glTarget,
                                           attachment.glTexture,
                                           attachment.level,
                                           attachment.layer );
                break;

            default:
                assert(!"Can't get here");
            }

#ifndef SGL_NO_STATUS_CHECK
            error = glGetError();
            if ( error != GL_NO_ERROR ) 
            {
                GuardedUnbind(oldFBO);
                return CheckGLError("GLRenderTarget::Dirty failed. Failed to attach color attachment: ", error);
            }
#endif
        }
    }

    // attach depth buffer
    if (dsAttachment.glTarget)
    {
        glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT,
                                   GL_DEPTH_ATTACHMENT_EXT,
                                   dsAttachment.glTarget,
                                   dsAttachment.glTexture,
                                   dsAttachment.level );

#ifndef SGL_NO_STATUS_CHECK
        error = glGetError();
        if ( error != GL_NO_ERROR ) 
        {
            GuardedUnbind(oldFBO);
            return CheckGLError("GLRenderTarget::Dirty failed. Failed to attach depth-stencil attachment: ", error);
        }
#endif
    }
    else if (useDepthStencilRenderbuffer)
    {
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, dsRenderBuffer);
        if ( dsRenderBufferAttachment.samples > 0 )
        {
            glRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER_EXT,
                                                 dsRenderBufferAttachment.samples,
                                                 BIND_GL_FORMAT[dsRenderBufferAttachment.format],
                                                 maxAttachmentWidth,
                                                 maxAttachmentHeight );
        }
        else
        {
            glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT,
                                      BIND_GL_FORMAT[dsRenderBufferAttachment.format],
                                      maxAttachmentWidth,
                                      maxAttachmentHeight );
        }

#ifndef SGL_NO_STATUS_CHECK
        error = glGetError();
        if ( error != GL_NO_ERROR ) 
        {
            GuardedUnbind(oldFBO);
            return CheckGLError("GLRenderTarget::Dirty failed. Failed to create depth-stencil renderbuffer: ", error);
        }
#endif

        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT,
                                      GL_DEPTH_ATTACHMENT_EXT, 
                                      GL_RENDERBUFFER_EXT, 
                                      dsRenderBuffer );
        glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT,
                                      GL_STENCIL_ATTACHMENT_EXT, 
                                      GL_RENDERBUFFER_EXT, 
                                      dsRenderBuffer );
    
#ifndef SGL_NO_STATUS_CHECK
        error = glGetError();
        if ( error != GL_NO_ERROR ) 
        {
            GuardedUnbind(oldFBO);
            return CheckGLError("GLRenderTarget::Dirty failed. Failed to attach depth-stencil renderbuffer: ", error);
        }
#endif
    }

#ifndef SGL_NO_STATUS_CHECK
    {
        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
        {
            GuardedUnbind(oldFBO);
            return CheckGLFramebufferStatus("GLRenderTarget::Dirty failed: ", status);
        }
    }
#endif

    dirty = false;
    return SGL_OK;
}

SGL_HRESULT GLRenderTarget::SetDrawBuffer(unsigned int drawBuffer)
{
    drawBuffers.resize(1);
    drawBuffers[0] = GL_COLOR_ATTACHMENT0_EXT + drawBuffer;
    if ( device->CurrentRenderTarget() == this ) {
        glDrawBuffer(drawBuffers[0]);
    }

    return SGL_OK;
}

SGL_HRESULT GLRenderTarget::SetDrawBuffers(unsigned int numTargets, unsigned int* targets)
{
    // copy draw buffers
    drawBuffers.resize(numTargets);
    std::transform( targets, 
                    targets + numTargets, 
                    drawBuffers.begin(), 
                    std::bind1st(std::plus<int>(), GL_COLOR_ATTACHMENT0) );

    // setup to openGL
    if ( device->CurrentRenderTarget() == this ) 
    {
        if ( drawBuffers.empty() ) {
            glDrawBuffer(GL_NONE);
        }
        else {
            glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);
        }
    }

    return SGL_OK;
}

SGL_HRESULT GLRenderTarget::SetReadBuffer(unsigned int _readBuffer)
{
    readBuffer = _readBuffer + GL_COLOR_ATTACHMENT0;
    if ( device->CurrentRenderTarget() == this ) {
        glReadBuffer(readBuffer);
    }

    return SGL_OK;
}

unsigned int GLRenderTarget::ReadBuffer() const
{
    return readBuffer - GL_COLOR_ATTACHMENT0_EXT;
}

unsigned int GLRenderTarget::DrawBuffers(unsigned int* targets) const
{
    if (targets) 
    {
        std::transform( drawBuffers.begin(), 
                        drawBuffers.end(), 
                        targets, 
                        std::bind1st(std::minus<int>(), GL_COLOR_ATTACHMENT0) );
    }

    return drawBuffers.size();
}

SGL_HRESULT GLRenderTarget::Bind() const
{
#ifndef SGL_NO_STATUS_CHECK
    if (dirty) {
        return EInvalidCall("GLRenderTarget::Bind failed. Render target is dirty.");
    }
#endif

    // bind buffer
    glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
    if ( drawBuffers.empty() ) {
        glDrawBuffer(GL_NONE);
    }
    else {
        glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);
    }
    glReadBuffer(readBuffer);
    static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetRenderTarget(this);

    return SGL_OK;
}

void GLRenderTarget::Unbind() const
{
    if ( device->CurrentRenderTarget() == this )
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
        static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetRenderTarget(0);
    }
}

} // namespace sgl
