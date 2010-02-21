#ifndef SIMPLE_GL_GL_DEVICE_TRAITS_H
#define SIMPLE_GL_GL_DEVICE_TRAITS_H

#include "Device.h"

namespace sgl {

/** DeviceTraits is used to extract information about supported device features.
 * !!!INCOMPLETE!!!
 */
class GLDeviceTraits :
	public ReferencedImpl<DeviceTraits>
{
public:
    GLDeviceTraits(Device* _device);

    // Override DeviceTraits
    int  SGL_DLLCALL ShaderModel() const { return shaderModel; }
    unsigned int SGL_DLLCALL NumberOfVertexTextureUnits() const { return numVertexTIU; }
    unsigned int SGL_DLLCALL NumberOfCombinedTextureUnits() const { return numCombinedTIU; }

    bool SGL_DLLCALL SupportsR2VB() const { return supportsR2VB; }
    bool SGL_DLLCALL SupportsSeparatedStencil() const { return supportsSeparateStencil; }
    bool SGL_DLLCALL SupportsNPOT() const { return supportsNPOT; }
    bool SGL_DLLCALL SupportsHardwareMipmap() const { return supportsHardwareMipmap; }

    unsigned int SGL_DLLCALL MaxTextureWidth() const { return maxTextureWidth; }
    unsigned int SGL_DLLCALL MaxTextureHeight() const { return maxTextureHeight; }

    unsigned int SGL_DLLCALL FindSupportedTextureFormats(Texture2D::FORMAT* formats) const;

private:
    ref_ptr<Device> device;

    // flags
    bool supportsFBO;
    bool supportsR2VB;
    bool supportsVertexTexelFetch;
    bool supportsSeparateStencil;
    bool supportsNPOT;
    bool supportsHardwareMipmap;

    // other values
    int  shaderModel;
    unsigned int numVertexTIU;
    unsigned int numCombinedTIU;
    unsigned int maxTextureWidth;
    unsigned int maxTextureHeight;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_DEVICE_TRAITS_H
