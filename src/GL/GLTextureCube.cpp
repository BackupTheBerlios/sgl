#include "GL/GLCommon.h"
#include "GL/GLTextureCube.h"

namespace sgl {

//================================= GLTextureCubeSide =================================//
template<DEVICE_VERSION DeviceVersion>
GLTextureCubeSide<DeviceVersion>::GLTextureCubeSide( GLTextureCube<DeviceVersion>*  texture,
                                                     const Texture2D::DESC&         desc,
                                                     TextureCube::SIDE              _side ) :
    GLTexture<DeviceVersion, Texture2D>(texture->device, GL_TEXTURE_CUBE_MAP, false),
    side(_side),
    format(desc.format),
    width(desc.width),
    height(desc.height)
{
    base_type::glTexture = texture->glTexture;

    // image settings
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];
    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, texture, 0) );
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

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTextureCubeSide<DeviceVersion>::SetSubImage( unsigned int  mipmap,
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
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, texture, 0) );
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

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTextureCubeSide<DeviceVersion>::GetImage( unsigned int  mipmap,
                                                        void*         data )
{
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];

    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, texture, 0) );
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
        return CheckGLError( "GLTextureCubeSide<DeviceVersion>::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}

//================================= GLTextureCube =================================//

template<DEVICE_VERSION DeviceVersion>
GLTextureCube<DeviceVersion>::GLTextureCube( GLDevice<DeviceVersion>*  device_,
                                             const TextureCube::DESC&  desc ) :
    GLTexture<DeviceVersion, TextureCube>(device_, GL_TEXTURE_CUBE_MAP)
{
    // create sides
    for (int i = 0; i<6; ++i) {
        sides[i].reset( new GLTextureCubeSide<DeviceVersion>( this, desc.sides[i], TextureCube::SIDE(i) ) );
    }
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTextureCube<DeviceVersion>::GenerateMipmap()
{
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, this, 0) );

#ifndef SGL_NO_STATUS_CHECK
    GLenum glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTextureCube::GenerateMipmap failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK


    if (glGenerateMipmapEXT) {
        glGenerateMipmapEXT(base_type::glTarget);
    }
    else {
        return EUnsupported("Hardware mipmap generation is not supported by the device");
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTextureCube::GenerateMipmap failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTextureCube<DeviceVersion>::BindSamplerState(SamplerState* _samplerState)
{
    base_type::samplerState.reset( static_cast<GLSamplerState<DeviceVersion>*>(_samplerState) );

    if (base_type::samplerState)
    {
        guarded_binding_ptr       guardedTexture( new guarded_binding(base_type::device, this, 0) );
        const SamplerState::DESC& desc = base_type::samplerState->Desc();

        glTexParameteri( base_type::glTarget, GL_TEXTURE_MIN_FILTER,           BIND_TEXTURE_MIN_FILTER[ desc.filter[2] * 3 + desc.filter[0] ] );
        glTexParameteri( base_type::glTarget, GL_TEXTURE_MAG_FILTER,           BIND_TEXTURE_FILTER[ desc.filter[1] ] );
        //glTexParameterf( base_type::glTarget, GL_TEXTURE_MIN_LOD,              desc.minLod );
        //glTexParameterf( base_type::glTarget, GL_TEXTURE_MAX_LOD,              desc.maxLod );
        //glTexParameterf( base_type::glTarget, GL_TEXTURE_LOD_BIAS,             desc.lodBias );
        glTexParameteri( base_type::glTarget, GL_TEXTURE_WRAP_S,               BIND_TEXTURE_CLAMP[ desc.wrapping[0] ] );
        glTexParameteri( base_type::glTarget, GL_TEXTURE_WRAP_T,               BIND_TEXTURE_CLAMP[ desc.wrapping[1] ] );
        glTexParameteri( base_type::glTarget, GL_TEXTURE_WRAP_R,               BIND_TEXTURE_CLAMP[ desc.wrapping[2] ] );
        //glTexParameteri( base_type::glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT,   desc.maxAnisotropy);
    }

    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTextureCube<DeviceVersion>::Bind(unsigned int stage_) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( base_type::stage >= Device::NUM_TEXTURE_STAGES ) {
        return EInvalidCall("GLTextureCube::Bind failed. Stage is too large.");
    }
#endif

    base_type::stage = stage_;
    glActiveTexture(GL_TEXTURE0 + base_type::stage);
    glEnable(base_type::glTarget);
    glBindTexture(base_type::glTarget, base_type::glTexture);

    base_type::device->SetTexture(base_type::stage, this);
    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
void GLTextureCube<DeviceVersion>::Unbind() const
{
    if ( base_type::stage >= 0 && base_type::device->CurrentTexture(base_type::stage) == this )
    {
        glActiveTexture(GL_TEXTURE0 + base_type::stage);
        glBindTexture(base_type::glTarget, 0);
        glDisable(base_type::glTarget);
        base_type::device->SetTexture(base_type::stage, 0);
        base_type::stage = -1;
    }
}

// explicit template instantiation
template class GLTextureCubeSide<DV_OPENGL_1_3>;
template class GLTextureCubeSide<DV_OPENGL_1_4>;
template class GLTextureCubeSide<DV_OPENGL_1_5>;
template class GLTextureCubeSide<DV_OPENGL_2_0>;
template class GLTextureCubeSide<DV_OPENGL_2_1>;
template class GLTextureCubeSide<DV_OPENGL_3_0>;
template class GLTextureCubeSide<DV_OPENGL_3_1>;
template class GLTextureCubeSide<DV_OPENGL_3_2>;

template class GLTextureCube<DV_OPENGL_1_3>;
template class GLTextureCube<DV_OPENGL_1_4>;
template class GLTextureCube<DV_OPENGL_1_5>;
template class GLTextureCube<DV_OPENGL_2_0>;
template class GLTextureCube<DV_OPENGL_2_1>;
template class GLTextureCube<DV_OPENGL_3_0>;
template class GLTextureCube<DV_OPENGL_3_1>;
template class GLTextureCube<DV_OPENGL_3_2>;

} // namespace sgl
