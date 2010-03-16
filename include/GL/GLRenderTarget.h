#ifndef SIMPLE_GL_GL_RENDER_TARGET_H
#define SIMPLE_GL_GL_RENDER_TARGET_H

#include "GLTexture2D.h"
#include "GLTexture3D.h"
#include "GLTextureCube.h"
#include <vector>

namespace sgl {

/* GL fbo wrapper */
class GLRenderTarget :
    public ReferencedImpl<RenderTarget>
{
private:
    typedef std::vector<GLuint> GLuint_vector;

    struct renderbuffer
    {
        Texture::FORMAT         format;
        unsigned int            samples;

        renderbuffer( Texture::FORMAT _format  = Texture::D24S8,
                      unsigned int    _samples = 0 ) :
            format(_format),
            samples(_samples)
        {}
    };

    struct attachment
    {
        ref_ptr<Texture>    texture;
        GLuint              glTarget;
        GLuint              glTexture;
        GLuint              width;
        GLuint              height;
        GLuint              level;
        GLint               layer;

        attachment( GLuint  _glTarget  = 0,
                    GLuint  _glTexture = 0,
                    GLuint  _width     = 0,
                    GLuint  _height    = 0,
                    GLuint  _level     = 0,
                    GLint   _layer     = -1 ) :
            glTarget(_glTarget),
            glTexture(_glTexture),
            width(_width),
            height(_height),
            level(_level),
            layer(_layer)
        {}

        attachment( GLTexture<Texture2D>*   _texture,
                    unsigned int            _level ) :
            texture(_texture),
            glTarget(_texture->glTarget),
            glTexture(_texture->glTexture),
            width( _texture->Width() ),
            height( _texture->Height() ),
            level(_level),
            layer(-1)
        {}

        attachment( GLTexture<Texture3D>*   _texture,
                    unsigned int            _level,
                    unsigned int            _layer ) :
            texture(_texture),
            glTarget(_texture->glTarget),
            glTexture(_texture->glTexture),
            width( _texture->Width() ),
            height( _texture->Height() ),
            level(_level),
            layer(_layer)
        {}
    };
    typedef std::vector<attachment>     attachment_vector;

    struct guarded_binding :
        public ReferencedImpl<Referenced>
    {
        guarded_binding(const Device* device, const GLRenderTarget* target)
        {
            assert(device && target);
            if (device->CurrentRenderTarget() != target)
            {
                renderTarget.reset( static_cast<const GLRenderTarget*>( device->CurrentRenderTarget() ) );
                target->Bind();
            }
        }

        ~guarded_binding()
        {
            if (renderTarget) {
                renderTarget->Bind();
            }
        }

        ref_ptr<const GLRenderTarget> renderTarget;
    };
    typedef ref_ptr<guarded_binding>    guarded_binding_ptr;

public:
    GLRenderTarget(Device* device);
    ~GLRenderTarget();

    // Override RenderTarget
    SGL_HRESULT     SGL_DLLCALL SetDepthStencil( bool             toggle,
                                                 Texture::FORMAT  format   = Texture::D24S8,
                                                 unsigned int     samples  = 0 );

    SGL_HRESULT     SGL_DLLCALL SetColorAttachment( unsigned int    /*mrtIndex*/,
                                                    Texture1D*      /*texture*/,
                                                    unsigned int    /*level*/ )     { return EUnsupported(); } // !

    SGL_HRESULT     SGL_DLLCALL SetColorAttachment( unsigned int    mrtIndex,
                                                    Texture2D*      texture,
                                                    unsigned int    level );

    SGL_HRESULT     SGL_DLLCALL SetColorAttachment( unsigned int    mrtIndex,
                                                    Texture3D*      texture,
                                                    unsigned int    level,
                                                    unsigned int    layer );

    SGL_HRESULT     SGL_DLLCALL SetColorAttachmentLayered( unsigned int    /*mrtIndex*/,
                                                           Texture3D*      /*texture*/,
                                                           unsigned int    /*level*/ )  { return EUnsupported(); } // !

    SGL_HRESULT     SGL_DLLCALL SetColorAttachmentLayered( unsigned int     /*mrtIndex*/,
                                                           TextureCube*     /*texture*/,
                                                           unsigned int     /*level*/ ) { return EUnsupported(); } // !

    SGL_HRESULT     SGL_DLLCALL SetDepthStencilAttachment( Texture2D*       depthStencilTexture,
                                                           unsigned int     level );

    SGL_HRESULT     SGL_DLLCALL SetDepthStencilAttachmentLayered( Texture3D*       /*depthStencilTexture*/,
                                                                  unsigned int     /*level*/ ) { return EUnsupported(); } // !

    SGL_HRESULT     SGL_DLLCALL SetDepthStencilAttachmentLayered( TextureCube*     /*depthStencilTexture*/,
                                                                  unsigned int     /*level*/ ) { return EUnsupported(); } // !

    Texture*        SGL_DLLCALL ColorAttachment(unsigned int mrtIndex) const;
    Texture*        SGL_DLLCALL DepthStencilAttachment() const;
    bool            SGL_DLLCALL HaveDepthStencil() const;
    bool            SGL_DLLCALL IsDirty() const;
    SGL_HRESULT     SGL_DLLCALL Dirty(bool force = false);

    SGL_HRESULT     SGL_DLLCALL SetReadBuffer(unsigned int target);
    SGL_HRESULT     SGL_DLLCALL SetDrawBuffer(unsigned int target);
    SGL_HRESULT     SGL_DLLCALL SetDrawBuffers( unsigned int    numTargets,
                                                unsigned int*   targets );

    unsigned int    SGL_DLLCALL ReadBuffer() const;
    unsigned int    SGL_DLLCALL DrawBuffers(unsigned int* targets) const;

    SGL_HRESULT     SGL_DLLCALL Bind() const;
    void            SGL_DLLCALL Unbind() const;

private:
    // device
    ref_ptr<Device>     device;

    // settings
    bool                dirty;
    bool                useDepthStencilRenderbuffer;

    // buffers
    attachment_vector   attachments;
    attachment          dsAttachment;
    renderbuffer        dsRenderBufferAttachment;

    // OpenGL
    GLuint              fbo;
    GLuint              dsRenderBuffer;
    GLuint_vector       drawBuffers;
    GLuint              readBuffer;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_RENDER_TARGET_H
