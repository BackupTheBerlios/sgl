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

/** bind glTexture environment mode to gl one */
extern const GLenum BIND_GL_TEXTURE_ENV_MODE[];

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
                         const GLTexture*   target,
                         unsigned int       stage_) :
            stage(stage_),
            texture(0)
        {
            assert(device && target);
            if ( device->CurrentTexture(stage) != target )
            {
                texture = device->CurrentTexture(stage);
                target->Bind(stage);
            }
        }

        ~guarded_binding()
        {
            if (texture) {
                texture->Bind(stage);
            }
        }

        int             stage;
        const Texture*  texture;
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

    guarded_binding_ptr GuardedBind(unsigned int stage) const
    {
        if ( device->CurrentTexture(stage) != this )
        {
            guarded_binding_ptr guardedTexture( new guarded_binding(device->CurrentTexture(stage), stage) );

            glActiveTexture(GL_TEXTURE0 + stage);
            glBindTexture(glTarget, glTexture);
            return guardedTexture;
        }

        return guarded_binding();
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
