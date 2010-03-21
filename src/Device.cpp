#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

using namespace sgl;

extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDevice(sgl::DEVICE_VERSION deviceVersion, const Device::VIDEO_DESC& desc)
{
    try
    {
        switch (deviceVersion)
        {
        case DV_OPENGL_1_3:
            return new GLDevice<DV_OPENGL_1_3>(desc);

        case DV_OPENGL_1_4:
            return new GLDevice<DV_OPENGL_1_4>(desc);

        case DV_OPENGL_1_5:
            return new GLDevice<DV_OPENGL_1_5>(desc);

        case DV_OPENGL_2_0:
            return new GLDevice<DV_OPENGL_2_0>(desc);

        case DV_OPENGL_2_1:
            return new GLDevice<DV_OPENGL_2_1>(desc);
            
        case DV_OPENGL_3_0:
            return new GLDevice<DV_OPENGL_3_0>(desc);

        case DV_OPENGL_3_1:
            return new GLDevice<DV_OPENGL_3_1>(desc);

        case DV_OPENGL_3_2:
            return new GLDevice<DV_OPENGL_3_2>(desc);

        default:
            return 0;
        }
    }
    catch(gl_error&) 
    {
        return 0;
    }
}

extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDeviceFromCurrent(sgl::DEVICE_VERSION deviceVersion)
{
    // init extensions
    static bool glewInitialized = false;
    if (!glewInitialized)
    {
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            sglSetError(SGLERR_UNKNOWN, "Can't init glew.");
            return 0;
        }

        std::cout << "OpenGL version is " << glGetString(GL_VERSION) << std::endl;
        glewInitialized = true;
    }

    try
    {
        switch (deviceVersion)
        {
        case DV_OPENGL_1_3:
            if (GLEW_VERSION_1_3) {
                return new GLDevice<DV_OPENGL_1_3>();
            }
            return 0;

        case DV_OPENGL_1_4:
            if (GLEW_VERSION_1_4) {
                return new GLDevice<DV_OPENGL_1_4>();
            }
            return 0;

        case DV_OPENGL_1_5:
            if (GLEW_VERSION_1_5) {
                return new GLDevice<DV_OPENGL_1_5>();
            }
            return 0;

        case DV_OPENGL_2_0:
            if (GLEW_VERSION_2_0) {
                return new GLDevice<DV_OPENGL_2_0>();
            }
            return 0;

        case DV_OPENGL_2_1:
            if (GLEW_VERSION_2_1) {
                return new GLDevice<DV_OPENGL_2_1>();
            }
            return 0;
            
        case DV_OPENGL_3_0:
            if (GLEW_VERSION_3_0) {
                return new GLDevice<DV_OPENGL_3_0>();
            }
            return 0;

        case DV_OPENGL_3_1:
            if (GLEW_VERSION_3_1) {
                return new GLDevice<DV_OPENGL_3_1>();
            }
            return 0;

        case DV_OPENGL_3_2:
            if (GLEW_VERSION_3_2) {
                return new GLDevice<DV_OPENGL_3_2>();
            }
            return 0;
            
        default:
            return 0;
        }
    }
    catch(gl_error&) 
    {
        return 0;
    }
}

// define scalar type traits
//const scalar_type_desc T::SCALAR_TYPE_TRAITS[]
