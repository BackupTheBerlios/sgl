#include "GL/GLCommon.h"
#include "GL/GLTexture3D.h"

namespace sgl {

GLTexture3D::GLTexture3D(GLDevice* device, const DESC& desc) :
    GLTexture<Texture3D>(device, GL_TEXTURE_3D),
    format(desc.format),
    width(desc.width),
    height(desc.height),
    depth(desc.depth),
    numSamples(0),
    numMipmaps(0)
{
    // image settings
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];
    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_texture_ptr guardedTexture = GuardedBind(0);
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture3D::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed) 
    {
        glCompressedTexImage3D( glTarget, 
                                0, 
                                glFormat, 
                                width, 
                                height, 
                                depth,
                                0, 
                                Image::SizeOfData(format, width, height, 1), 
                                desc.data );
    }
    else 
    {
        glTexImage3D( glTarget, 
                      0, 
                      glFormat, 
                      width,
                      height,
                      depth,
                      0, 
                      glUsage, 
                      glPixelType, 
                      desc.data );
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture3D::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK
}

GLTexture3D::GLTexture3D(GLDevice* device, const DESC_MS& desc) :
    GLTexture<Texture3D>(device, 0xFFFFFF), // FIXME
    format(desc.format),
    width(desc.width),
    height(desc.height),
    depth(desc.depth),
    numSamples(desc.samples),
    numMipmaps(0)
{
    throw gl_error("GLTexture3D::GLTexture3D failed: Glew doesn't support 3D multismapled textures.");
    /*
    // image settings
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];
    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_texture_ptr guardedTexture = GuardedBind(0);
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture3D::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK

    // create image
    glTexImage3DMultisample( glTarget, 
                             numSamples, 
                             glFormat, 
                             width, 
                             height, 
                             depth,
                             GL_TRUE );

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture3D::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK
    */
}

SGL_HRESULT GLTexture3D::SetSubImage( unsigned int  mipmap,
                                      unsigned int  offsetx,
                                      unsigned int  offsety,
                                      unsigned int  offsetz,
                                      unsigned int  regionWidth, 
                                      unsigned int  regionHeight, 
                                      unsigned int  regionDepth,
                                      const void*   data )
{
    // image settings
    GLenum glError;
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];
    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_texture_ptr guardedTexture = GuardedBind(0);
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed) 
    {
        glCompressedTexSubImage3D( glTarget, 
                                   mipmap, 
                                   offsetx,
                                   offsety,
                                   offsetz,
                                   regionWidth, 
                                   regionHeight,
                                   regionDepth,
                                   glFormat, 
                                   Image::SizeOfData(format, regionWidth, regionHeight, 1), 
                                   data );
    }
    else 
    {
        glTexSubImage3D( glTarget, 
                         mipmap, 
                         offsetx,
                         offsety,
                         offsetz,
                         regionWidth, 
                         regionHeight, 
                         regionDepth,
                         glFormat, 
                         glPixelType, 
                         data );
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK


    // remember mipmaping
    numMipmaps = std::max(numMipmaps, mipmap);
    return SGL_OK;
}

SGL_HRESULT GLTexture3D::GetImage( unsigned int  mipmap,
                                   void*         data )
{
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];

    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_texture_ptr guardedTexture = GuardedBind(0);
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glGetCompressedTexImage(glTarget,
                                mipmap,
                                data);
    }
    else
    {
        glGetTexImage(glTarget,
                      mipmap,
                      glUsage,
                      glPixelType,
                      data);
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}

SGL_HRESULT GLTexture3D::GenerateMipmap()
{
    guarded_texture_ptr guardedTexture = GuardedBind(0);

#ifndef SGL_NO_STATUS_CHECK
    GLenum glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D::GenerateMipmap failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    glGenerateMipmap(glTarget);
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D::GenerateMipmap failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}

SGL_HRESULT GLTexture3D::Bind(unsigned int _stage) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( stage >= Device::NUM_TEXTURE_STAGES ) {
        return EInvalidCall("GLTexture3D::Bind failed. Stage is too large.");
    }
#endif

    stage = _stage;
    glActiveTexture(GL_TEXTURE0 + stage);
    glBindTexture(glTarget, glTexture);

    if ( boundSamplerState != device->CurrentSamplerState(stage) ) 
    {
        boundSamplerState               = static_cast<const GLSamplerState*>( device->CurrentSamplerState(stage) );
        const SamplerState::DESC& desc  = boundSamplerState->Desc();
        if (numSamples == 0) 
        {
            glTexParameteri( glTarget, GL_TEXTURE_MIN_FILTER,       BIND_TEXTURE_MIN_FILTER[ (desc.filter[0] << 1) | (desc.filter[2]) ] );
            glTexParameteri( glTarget, GL_TEXTURE_MAG_FILTER,       BIND_TEXTURE_FILTER[ desc.filter[1] ] );
            glTexParameterf( glTarget, GL_TEXTURE_MIN_LOD,          desc.minLod );
            glTexParameterf( glTarget, GL_TEXTURE_MAX_LOD,          desc.maxLod );
            glTexParameterf( glTarget, GL_TEXTURE_LOD_BIAS,         desc.lodBias );

        }

        glTexParameteri( glTarget, GL_TEXTURE_WRAP_S,               BIND_TEXTURE_CLAMP[ desc.wrapping[0] ] );
        glTexParameteri( glTarget, GL_TEXTURE_WRAP_T,               BIND_TEXTURE_CLAMP[ desc.wrapping[1] ] );
        glTexParameteri( glTarget, GL_TEXTURE_WRAP_R,               BIND_TEXTURE_CLAMP[ desc.wrapping[2] ] );
        glTexParameteri( glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT,   desc.maxAnisotropy);
    }

    device->SetTexture(stage, this);
    return SGL_OK;
}

void GLTexture3D::Unbind() const
{
    if ( stage > 0 && device->CurrentTexture(stage) == this ) 
    {
        glActiveTexture(GL_TEXTURE0 + stage);
        glBindTexture(glTarget, 0);
        device->SetTexture(stage, 0);
        stage = -1;
    }
}

} // namespace sgl