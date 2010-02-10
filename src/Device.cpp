#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

using namespace sgl;

extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDevice(sgl::DEVICE_TYPE devType, const Device::VIDEO_DESC& desc)
{
    try
    {
        if (devType == OPENGL_DEVICE) {
            return new GLDevice(desc);
        }
    }
    catch(gl_error&) 
    {
        return 0;
    }

    assert(!"You can't get here");
    return 0;
}

extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDeviceFromCurrent(sgl::DEVICE_TYPE devType)
{
    try
    {
        if (devType == OPENGL_DEVICE) {
            return new GLDevice();
        }
    }
    catch(gl_error&) 
    {
        return 0;
    }

    assert(!"You can't get here");
    return 0;
}

// define scalar type traits
//const scalar_type_desc T::SCALAR_TYPE_TRAITS[]
