#include "GL/GLCommon.h"
#include "GL/GLTexture2D.h"

namespace sgl {

GLTexture2D::GLTexture2D(GLDevice* device, const Texture2D::DESC& desc) :
    GLTexture<Texture2D>(device, GL_TEXTURE_2D),
    format(desc.format),
    width(desc.width),
    height(desc.height),
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
    guarded_binding_ptr guardedTexture( new guarded_binding(device, this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture2D<DeviceVersion>::GLTexture2D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glCompressedTexImage2D( glTarget,
                                0,
                                glFormat,
                                width,
                                height,
                                0,
                                Image::SizeOfData(format, width, height, 1),
                                desc.data );
    }
    else
    {
        glTexImage2D( glTarget,
                      0,
                      glFormat,
                      width,
                      height,
                      0,
                      glUsage,
                      glPixelType,
                      desc.data );
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture2D<DeviceVersion>::GLTexture2D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK
}

#ifndef SIMPLE_GL_ES
GLTexture2D::GLTexture2D(GLDevice* device, const Texture2D::DESC_MS& desc) :
    GLTexture<Texture2D>(device, GL_TEXTURE_2D_MULTISAMPLE),
    format(desc.format),
    width(desc.width),
    height(desc.height),
    numSamples(desc.samples),
    numMipmaps(0)
{
    // image settings
    GLenum glError;
    GLenum glFormat = BIND_GL_FORMAT[format];

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(device, this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture2D<DeviceVersion>::GLTexture2D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK

    // create image
    glTexImage2DMultisample( glTarget,
                             numSamples,
                             glFormat,
                             width,
                             height,
                             GL_TRUE );

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture2D<DeviceVersion>::GLTexture2D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK
}
#endif // !defined(SIMPLE_GL_ES)

GLTexture2D::~GLTexture2D()
{
    if (device->Valid()) {
    	Unbind();
    }
}

SGL_HRESULT GLTexture2D::SetSubImage( unsigned int  mipmap,
                                      unsigned int  offsetx,
                                      unsigned int  offsety,
                                      unsigned int  regionWidth,
                                      unsigned int  regionHeight,
                                      const void*   data )
{
    // image settings
    GLenum glError;
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];
    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(device, this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture2D<DeviceVersion>::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glCompressedTexSubImage2D( glTarget,
                                   mipmap,
                                   offsetx,
                                   offsety,
                                   regionWidth,
                                   regionHeight,
                                   glFormat,
                                   Image::SizeOfData(format, regionWidth, regionHeight, 1),
                                   data );
    }
    else
    {
        glTexSubImage2D( glTarget,
                         mipmap,
                         offsetx,
                         offsety,
                         regionWidth,
                         regionHeight,
                         glFormat,
                         glPixelType,
                         data );
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture2D<DeviceVersion>::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK


    // remember mipmaping
    numMipmaps = std::max(numMipmaps, mipmap);
    return SGL_OK;
}

#ifdef SIMPLE_GL_ES
SGL_HRESULT GLTexture2D::GetImage( unsigned int  /*mipmap*/,
                                   void*         /*data*/ )
{
    return EUnsupported("GLTexture2D::GetImage failed. Unsupported in GLES.");
}
#else
SGL_HRESULT GLTexture2D::GetImage( unsigned int  mipmap,
                                   void*         data )
{
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];

    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(device, this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture2D<DeviceVersion>::GetImage failed: ", glError );
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
        return CheckGLError( "GLTexture2D<DeviceVersion>::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}
#endif

SGL_HRESULT GLTexture2D::GenerateMipmap()
{
    guarded_binding_ptr guardedTexture( new guarded_binding(device, this, 0) );

#ifndef SGL_NO_STATUS_CHECK
    GLenum glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture2D<DeviceVersion>::GenerateMipmap failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

#ifdef SIMPLE_GL_ES
    glGenerateMipmap(glTarget);
#else
    if (glGenerateMipmapEXT) {
        glGenerateMipmapEXT(glTarget);
    }
    else 
    {
        size_t dataSize = Image::SizeOfData(format, width, height, 1);
        void*  data = malloc(dataSize);
        if (!data) {
            return EOutOfMemory("GLTexture2D::GenerateMipmap failed: out of memory");
        }

        SGL_HRESULT result = GetImage(0, data);
        if (result != SGL_OK) {
            return result;
        }

        GLint gluError = gluBuild2DMipmaps( GL_TEXTURE_2D,
                                            BIND_GL_FORMAT[format],
                                            width,
                                            height,
                                            BIND_GL_FORMAT_USAGE[format],
                                            BIND_GL_FORMAT_PIXEL_TYPE[format],
                                            data );
    #ifndef SGL_NO_STATUS_CHECK
        if ( gluError != GL_NO_ERROR ) {
            return CheckGLUError( "GLTexture2D::GenerateMipmap failed: ", gluError );
        }
    #endif // SGL_NO_STATUS_CHECK
    }
#endif

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture2D<DeviceVersion>::GenerateMipmap failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}

SGL_HRESULT GLTexture2D::BindSamplerState(sgl::SamplerState* _samplerState)
{
    samplerState.reset( static_cast<GLSamplerState*>(_samplerState) );

    if (samplerState)
    {
        guarded_binding_ptr guardedTexture( new guarded_binding(device, this, 0) );
        const SamplerState::DESC& desc = samplerState->Desc();
        if (numSamples == 0)
        {
            glTexParameteri( glTarget, GL_TEXTURE_MIN_FILTER,       BIND_TEXTURE_MIN_FILTER[ desc.filter[2] * 3 + desc.filter[0] ] );
            glTexParameteri( glTarget, GL_TEXTURE_MAG_FILTER,       BIND_TEXTURE_FILTER[ desc.filter[1] ] );
            //glTexParameterf( glTarget, GL_TEXTURE_MIN_LOD,          desc.minLod );
            //glTexParameterf( glTarget, GL_TEXTURE_MAX_LOD,          desc.maxLod );
            //glTexParameterf( glTarget, GL_TEXTURE_LOD_BIAS,         desc.lodBias );
        }

        glTexParameteri( glTarget, GL_TEXTURE_WRAP_S,               BIND_TEXTURE_CLAMP[ desc.wrapping[0] ] );
        glTexParameteri( glTarget, GL_TEXTURE_WRAP_T,               BIND_TEXTURE_CLAMP[ desc.wrapping[1] ] );
        //glTexParameteri( glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT,   desc.maxAnisotropy);
    }

    return SGL_OK;
}

SGL_HRESULT GLTexture2D::Bind(unsigned int stage_) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( stage >= Device::NUM_TEXTURE_STAGES ) {
        return EInvalidCall("GLTexture2D<DeviceVersion>::Bind failed. Stage is too large.");
    }
#endif

    stage = stage_;
    glActiveTexture(GL_TEXTURE0 + stage);
    glEnable(glTarget);
    glBindTexture(glTarget, glTexture);

    device->SetTexture(stage, this);
    return SGL_OK;
}

void GLTexture2D::Unbind() const
{
    if ( stage >= 0 && device->CurrentTexture(stage) == this )
    {
        glActiveTexture(GL_TEXTURE0 + stage);
        glBindTexture(glTarget, 0);
        glDisable(glTarget);
        device->SetTexture(stage, 0);
        stage = -1;
    }
}


} // namespace sgl
