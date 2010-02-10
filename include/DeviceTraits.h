#ifndef SIMPLE_GL_DEVICE_TRAITS_H
#define SIMPLE_GL_DEVICE_TRAITS_H

#include "Texture2D.h"

namespace sgl {

/** DeviceTraits is used to extract information about supported device features */
class DeviceTraits :
	public Referenced
{
public:
    /** Get latest shader model supported by the device
     * @return 3-digit integer corresponding supported shader model. E.g. 140 means 1.4.0
     * 0 means shaders are not supported.
     */
    virtual int SGL_DLLCALL ShaderModel() const = 0;

    /** Get supported maximum number of the textures in the vertex shader. */
    virtual unsigned int SGL_DLLCALL NumberOfVertexTextureUnits() const = 0;

    /** Get supported maximum number of the textures in the vertex/geometry/fragment shaders. */
    virtual unsigned int SGL_DLLCALL NumberOfCombinedTextureUnits() const = 0;

    /** Check whether device supports R2VB extension. */
    virtual bool SGL_DLLCALL SupportsR2VB() const = 0;

    /** Check wheter device supports separated stencil. */
    virtual bool SGL_DLLCALL SupportsSeparatedStencil() const = 0;

    /** Check whether device supports non power of two textures. */
    virtual bool SGL_DLLCALL SupportsNPOT() const = 0;

    /** Check whether device supports hardware mipmap generation. */
    virtual bool SGL_DLLCALL SupportsHardwareMipmap() const = 0;

    /** Get maximum texture width supported by the device. */
    virtual unsigned int SGL_DLLCALL MaxTextureWidth() const = 0;

    /** Get maximum texture height supported by the device. */
    virtual unsigned int SGL_DLLCALL MaxTextureHeight() const = 0;

    virtual SGL_DLLCALL ~DeviceTraits() {}
};

} // namespace sgl

#endif // SIMPLE_GL_DEVICE_TRAITS_H
