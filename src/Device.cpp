#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

using namespace sgl;

extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDevice(sgl::DEVICE_VERSION deviceVersion, const Device::VIDEO_DESC& desc)
{
    try
    {
        switch (deviceVersion)
        {
        case DV_OPENGL_2_1_MIXED:
            return new GLDevice<DV_OPENGL_2_1_MIXED>(desc);

        case DV_OPENGL_2_1_PROGRAMMABLE:
            return new GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>(desc);
            
        default:
            return 0;
        }
    }
    catch(gl_error&) 
    {
        return 0;
    }

    assert(!"You can't get here");
    return 0;
}

extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDeviceFromCurrent(sgl::DEVICE_VERSION deviceVersion)
{
    try
    {
        switch (deviceVersion)
        {
        case DV_OPENGL_2_1_MIXED:
            return new GLDevice<DV_OPENGL_2_1_MIXED>();

        case DV_OPENGL_2_1_PROGRAMMABLE:
            return new GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>();
            
        default:
            return 0;
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
