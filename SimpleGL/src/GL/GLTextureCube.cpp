#include "GL/GLCommon.h"
#include "GL/GLTextureCube.h"

namespace sgl {

//================================= GLTextureCubeSide =================================//
GLTextureCubeSide::GLTextureCubeSide( GLTextureCube*		 texture_,
                                      const Texture2D::DESC& desc,
                                      TextureCube::SIDE      side_ ) :
    GLTexture<Texture2D>(texture_->device, GL_TEXTURE_CUBE_MAP, false),
    texture(texture_),
    side(side_),
    format(desc.format),
    width(desc.width),
    height(desc.height)
{
    glTexture = texture->glTexture;

    // image settings
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];
    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(device, texture, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTextureCubeSide<DeviceVersion>::GLTextureCubeSide failed: ");
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
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
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
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
        throw gl_error("GLTextureCubeSide<DeviceVersion>::GLTextureCubeSide failed: ");
    }
#endif // SGL_NO_STATUS_CHECK
}

SGL_HRESULT GLTextureCubeSide::SetSubImage( unsigned int  mipmap,
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
    guarded_binding_ptr guardedTexture( new guarded_binding(device, texture, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTextureCubeSide<DeviceVersion>::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glCompressedTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
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
        glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
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
        return CheckGLError( "GLTextureCubeSide<DeviceVersion>::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK


    // remember mipmaping
    return SGL_OK;
}

#ifdef SIMPLE_GL_ES
SGL_HRESULT GLTextureCubeSide::GetImage( unsigned int  /*mipmap*/,
                                         void*         /*data*/ )
{
    return EUnsupported("GLTextureCubeSide::GetImage failed. Not supported in GLES.");
}
#else
SGL_HRESULT GLTextureCubeSide::GetImage( unsigned int  mipmap,
                                         void*         data )
{
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];

    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(device, texture, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTextureCubeSide<DeviceVersion>::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glGetCompressedTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
                                mipmap,
                                data);
    }
    else
    {
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
                      mipmap,
                      glUsage,
                      glPixelType,
                      data);
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTextureCubeSide::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}
#endif

//================================= GLTextureCube =================================//

GLTextureCube::GLTextureCube( GLDevice*  device_,
                              const TextureCube::DESC&  desc ) :
    GLTexture<TextureCube>(device_, GL_TEXTURE_CUBE_MAP)
{
    // create sides
    for (int i = 0; i<6; ++i) {
        sides[i].reset( new GLTextureCubeSide( this, desc.sides[i], TextureCube::SIDE(i) ) );
    }
}

SGL_HRESULT GLTextureCube::GenerateMipmap()
{
    guarded_binding_ptr guardedTexture( new guarded_binding(device, this, 0) );

#ifdef SIMPLE_GL_ES
    glGenerateMipmap(glTarget);
#else
    if (glGenerateMipmapEXT) 
    {
        glGenerateMipmapEXT(glTarget);
    #ifndef SGL_NO_STATUS_CHECK
        GLenum glError = glGetError();
        if ( glError != GL_NO_ERROR ) {
            return CheckGLError( "GLTextureCube::GenerateMipmap failed: ", glError );
        }
    #endif // SGL_NO_STATUS_CHECK
    }
    else 
    {
        for (int i = 0; i<6; ++i) 
        {
            GLTextureCubeSide* side = sides[i];
            
            size_t dataSize = Image::SizeOfData(side->format, side->width, side->height, 1);
            void*  data = malloc(dataSize);
            if (!data) {
                return EOutOfMemory("GLTextureCube::GenerateMipmap failed: out of memory");
            }

            SGL_HRESULT result = side->GetImage(0, data);
            if (result != SGL_OK) {
                return result;
            }

            GLint gluError = gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                                BIND_GL_FORMAT[side->format],
                                                side->width,
                                                side->height,
                                                BIND_GL_FORMAT_USAGE[side->format],
                                                BIND_GL_FORMAT_PIXEL_TYPE[side->format],
                                                data );
        #ifndef SGL_NO_STATUS_CHECK
            if ( gluError != GL_NO_ERROR ) {
                return CheckGLUError( "GLTextureCube::GenerateMipmap failed: ", gluError );
            }
        #endif // SGL_NO_STATUS_CHECK
        }
    }
#endif

    return SGL_OK;
}

GLTextureCube::~GLTextureCube()
{
    Unbind();
}

SGL_HRESULT GLTextureCube::BindSamplerState(SamplerState* _samplerState)
{
    samplerState.reset( static_cast<GLSamplerState*>(_samplerState) );

    if (samplerState)
    {
        guarded_binding_ptr       guardedTexture( new guarded_binding(device, this, 0) );
        const SamplerState::DESC& desc = samplerState->Desc();

        glTexParameteri( glTarget, GL_TEXTURE_MIN_FILTER,           BIND_TEXTURE_MIN_FILTER[ desc.filter[2] * 3 + desc.filter[0] ] );
        glTexParameteri( glTarget, GL_TEXTURE_MAG_FILTER,           BIND_TEXTURE_FILTER[ desc.filter[1] ] );
        //glTexParameterf( glTarget, GL_TEXTURE_MIN_LOD,              desc.minLod );
        //glTexParameterf( glTarget, GL_TEXTURE_MAX_LOD,              desc.maxLod );
        //glTexParameterf( glTarget, GL_TEXTURE_LOD_BIAS,             desc.lodBias );
        glTexParameteri( glTarget, GL_TEXTURE_WRAP_S,               BIND_TEXTURE_CLAMP[ desc.wrapping[0] ] );
        glTexParameteri( glTarget, GL_TEXTURE_WRAP_T,               BIND_TEXTURE_CLAMP[ desc.wrapping[1] ] );
        //glTexParameteri( glTarget, GL_TEXTURE_WRAP_R,               BIND_TEXTURE_CLAMP[ desc.wrapping[2] ] );
        //glTexParameteri( glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT,   desc.maxAnisotropy);
    }

    return SGL_OK;
}

SGL_HRESULT GLTextureCube::Bind(unsigned int stage_) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( stage >= Device::NUM_TEXTURE_STAGES ) {
        return EInvalidCall("GLTextureCube::Bind failed. Stage is too large.");
    }
#endif

    stage = stage_;
    glActiveTexture(GL_TEXTURE0 + stage);
    glEnable(glTarget);
    glBindTexture(glTarget, glTexture);

    device->SetTexture(stage, this);
    return SGL_OK;
}

void GLTextureCube::Unbind() const
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
