#include "GL/GLCommon.h"
#include "GL/GLTexture3D.h"

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
GLTexture3D<DeviceVersion>::GLTexture3D(GLDevice<DeviceVersion>* device_, const Texture3D::DESC& desc) :
    GLTexture<DeviceVersion, Texture3D>(device_, GL_TEXTURE_3D),
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
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture3D<DeviceVersion>::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glCompressedTexImage3D( base_type::glTarget,
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
        glTexImage3D( base_type::glTarget,
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
        throw gl_error("GLTexture3D<DeviceVersion>::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK
}

template<DEVICE_VERSION DeviceVersion>
GLTexture3D<DeviceVersion>::GLTexture3D(GLDevice<DeviceVersion>* device_, const Texture3D::DESC_MS& desc) :
    GLTexture<DeviceVersion, Texture3D>(device_, 0xFFFFFF), // FIXME
    format(desc.format),
    width(desc.width),
    height(desc.height),
    depth(desc.depth),
    numSamples(desc.samples),
    numMipmaps(0)
{
    throw gl_error("GLTexture3D<DeviceVersion>::GLTexture3D failed: Glew doesn't support 3D multismapled textures.");
    /*
    // image settings
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat    = BIND_GL_FORMAT[format];
    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device.get(), this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture3D<DeviceVersion>::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK

    // create image
    glTexImage3DMultisample( base_type::glTarget,
                             numSamples,
                             glFormat,
                             width,
                             height,
                             depth,
                             GL_TRUE );

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        throw gl_error("GLTexture3D<DeviceVersion>::GLTexture3D failed: ");
    }
#endif // SGL_NO_STATUS_CHECK
    */
}

template<DEVICE_VERSION DeviceVersion>
GLTexture3D<DeviceVersion>::~GLTexture3D()
{
    Unbind();
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTexture3D<DeviceVersion>::SetSubImage( unsigned int  mipmap,
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
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D<DeviceVersion>::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glCompressedTexSubImage3D( base_type::glTarget,
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
        glTexSubImage3D( base_type::glTarget,
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
        return CheckGLError( "GLTexture3D<DeviceVersion>::SetSubImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK


    // remember mipmaping
    numMipmaps = std::max(numMipmaps, mipmap);
    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTexture3D<DeviceVersion>::GetImage( unsigned int  mipmap,
                                                  void*         data )
{
    GLenum glError;
	GLenum glUsage     = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];

    bool   compressed  = Texture::FORMAT_TRAITS[format].compressed;

    // save previous state & bind texture
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, this, 0) );
#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D<DeviceVersion>::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    // copy image
    if (compressed)
    {
        glGetCompressedTexImage(base_type::glTarget,
                                mipmap,
                                data);
    }
    else
    {
        glGetTexImage(base_type::glTarget,
                      mipmap,
                      glUsage,
                      glPixelType,
                      data);
    }

#ifndef SGL_NO_STATUS_CHECK
    glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D<DeviceVersion>::GetImage failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTexture3D<DeviceVersion>::GenerateMipmap()
{
    guarded_binding_ptr guardedTexture( new guarded_binding(base_type::device, this, 0) );

#ifndef SGL_NO_STATUS_CHECK
    GLenum glError = glGetError();
    if ( glError != GL_NO_ERROR ) {
        return CheckGLError( "GLTexture3D<DeviceVersion>::GenerateMipmap failed: ", glError );
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
        return CheckGLError( "GLTexture3D<DeviceVersion>::GenerateMipmap failed: ", glError );
    }
#endif // SGL_NO_STATUS_CHECK

    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLTexture3D<DeviceVersion>::Bind(unsigned int stage_) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( base_type::stage >= Device::NUM_TEXTURE_STAGES ) {
        return EInvalidCall("GLTexture3D<DeviceVersion>::Bind failed. Stage is too large.");
    }
#endif

    base_type::stage = stage_;
    glActiveTexture(GL_TEXTURE0 + base_type::stage);
    glBindTexture(base_type::glTarget, base_type::glTexture);
    base_type::device->SetTexture(base_type::stage, this);

    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
void GLTexture3D<DeviceVersion>::Unbind() const
{
    if ( base_type::stage > 0 && base_type::device->CurrentTexture(base_type::stage) == this )
    {
        glActiveTexture(GL_TEXTURE0 + base_type::stage);
        glBindTexture(base_type::glTarget, 0);
        base_type::device->SetTexture(base_type::stage, 0);
        base_type::stage = -1;
    }
}

// explicit template instantiation
template class GLTexture3D<DV_OPENGL_1_3>;
template class GLTexture3D<DV_OPENGL_1_4>;
template class GLTexture3D<DV_OPENGL_1_5>;
template class GLTexture3D<DV_OPENGL_2_0>;
template class GLTexture3D<DV_OPENGL_2_1>;
template class GLTexture3D<DV_OPENGL_3_0>;
template class GLTexture3D<DV_OPENGL_3_1>;
template class GLTexture3D<DV_OPENGL_3_2>;

} // namespace sgl
