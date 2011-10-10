#ifndef SIMPLE_GL_GL_TEXTURE_H
#define SIMPLE_GL_GL_TEXTURE_H

#include "GLDevice.h"
#include "GLSamplerState.h"
#include <string>

#ifdef SIMPLE_GL_USE_SDL_IMAGE
#   include <SDL_image.h>
#endif

namespace sgl {

/** bind glTexture format to gl one */
extern const GLenum BIND_GL_FORMAT[];

/** Bind sgl format usage gl one */
extern const GLenum BIND_GL_FORMAT_USAGE[];

/** Bind sgl pixel type to gl one */
extern const GLenum BIND_GL_FORMAT_PIXEL_TYPE[];

/** bind glTexture filter arg gl one */
extern const GLenum BIND_TEXTURE_CLAMP[];

/** bind glTexture filter to gl one */
extern const GLenum BIND_TEXTURE_MIN_FILTER[];

/** bind glTexture filter to gl one */
extern const GLenum BIND_TEXTURE_FILTER[];

/** Find openGL usage correspondign sgl format */
GLenum FindGLUsage(Texture::FORMAT format);

#ifdef SIMPLE_GL_USE_SDL_IMAGE
/** Get sgl format from SDL pixel format */
Texture::FORMAT FindTextureFormat(const SDL_PixelFormat& format);
#endif

// Class for modifying texture data content
template<typename Interface>
class GLTexture :
    public ReferencedImpl<Interface>
{
protected:
    typedef ReferencedImpl<Interface> base_type;

public:
    typedef GLSamplerState sampler_state_type;

    struct guarded_binding :
        public ReferencedImpl<Referenced>
    {
        guarded_binding( const Device*      device,
                         const GLTexture*   texture,
                         unsigned int       stage_) :
            stage(stage_),
            prevTexture(0),
            glTarget(texture->glTarget)
        {
            assert(device && texture);
            if ( device->CurrentTexture(stage) != texture )
            {
                prevTexture = device->CurrentTexture(stage);
                texture->Bind(stage);
            }
        }

        ~guarded_binding()
        {
            if (prevTexture) 
            {
                glActiveTexture(GL_TEXTURE0 + stage);
                glDisable(glTarget);
                prevTexture->Bind(stage);
            }
            else 
            {
                glActiveTexture(GL_TEXTURE0 + stage);
                glDisable(glTarget);
            }
        }

        int             stage;
        int             glTarget;
        const Texture*  prevTexture;
    };
    typedef ref_ptr<guarded_binding>    guarded_binding_ptr;

public:
    GLTexture(  GLDevice*	device_,
                GLuint      glTarget_,
                bool        generateTexture_ = true ) :
        device(device_),
        glTarget(glTarget_),
        generateTexture(generateTexture_),
        stage(-1)
    {
        if (generateTexture) {
            glGenTextures(1, &glTexture);
        }
    }

    GLuint Target() const   { return glTarget; }
    GLuint Handle() const   { return glTexture; }

protected:
    ~GLTexture()
    {
        if (generateTexture) {
            glDeleteTextures(1, &glTexture);
        }
    }

protected:
    // resource
    GLDevice* device;

    // OpenGL
    GLuint                  glTarget;
    GLuint                  glTexture;
    bool                    generateTexture;
    ref_ptr<GLSamplerState>	samplerState;

    // binding
    mutable int stage;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_TEXTURE_H
