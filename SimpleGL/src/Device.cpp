#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

using namespace sgl;

#ifndef __ANDROID__
extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDevice(sgl::DEVICE_VERSION deviceVersion, const Device::VIDEO_DESC& desc)
{
    try
    {
        switch (deviceVersion)
        {
        #ifndef SIMPLE_GL_ES
            case DV_OPENGL_1_3:
                return new GLDeviceConcrete<DV_OPENGL_1_3>(desc);

            case DV_OPENGL_1_4:
                return new GLDeviceConcrete<DV_OPENGL_1_4>(desc);

            case DV_OPENGL_1_5:
                return new GLDeviceConcrete<DV_OPENGL_1_5>(desc);

            case DV_OPENGL_2_0:
                return new GLDeviceConcrete<DV_OPENGL_2_0>(desc);

            case DV_OPENGL_2_1:
                return new GLDeviceConcrete<DV_OPENGL_2_1>(desc);

            case DV_OPENGL_3_0:
                return new GLDeviceConcrete<DV_OPENGL_3_0>(desc);

            case DV_OPENGL_3_1:
                return new GLDeviceConcrete<DV_OPENGL_3_1>(desc);

            case DV_OPENGL_3_2:
                return new GLDeviceConcrete<DV_OPENGL_3_2>(desc);
        #else
            case DV_OPENGL_ES_1_0:
                return new GLDeviceConcrete<DV_OPENGL_ES_1_0>(desc);

            case DV_OPENGL_ES_1_1:
                return new GLDeviceConcrete<DV_OPENGL_ES_1_1>(desc);

            case DV_OPENGL_ES_2_0:
                return new GLDeviceConcrete<DV_OPENGL_ES_2_0>(desc);
        #endif

        default:
            return 0;
        }
    }
    catch(gl_error&) 
    {
        return 0;
    }
}
#endif // !defined(ANDROID)

extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDeviceFromCurrent(sgl::DEVICE_VERSION deviceVersion)
{
#ifdef SIMPLE_GL_ES
    switch (deviceVersion)
    {
    case DV_OPENGL_ES_1_0:
        try
        {
            return new GLDeviceConcrete<DV_OPENGL_ES_1_0>();
        }
        catch (gl_error&) {
            return 0;
        }

    case DV_OPENGL_ES_1_1:
        try
        {
            return new GLDeviceConcrete<DV_OPENGL_ES_1_1>();
        }
        catch (gl_error&) {
            return 0;
        }

    case DV_OPENGL_ES_2_0:
        try
        {
            return new GLDeviceConcrete<DV_OPENGL_ES_2_0>();
        }
        catch (gl_error&) {
            return 0;
        }

    default:
        return 0;
    }
#else
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
                return new GLDeviceConcrete<DV_OPENGL_1_3>();
            }
            return 0;

        case DV_OPENGL_1_4:
            if (GLEW_VERSION_1_4) {
                return new GLDeviceConcrete<DV_OPENGL_1_4>();
            }
            return 0;

        case DV_OPENGL_1_5:
            if (GLEW_VERSION_1_5) {
                return new GLDeviceConcrete<DV_OPENGL_1_5>();
            }
            return 0;

        case DV_OPENGL_2_0:
            if (GLEW_VERSION_2_0) {
                return new GLDeviceConcrete<DV_OPENGL_2_0>();
            }
            return 0;

        case DV_OPENGL_2_1:
            if (GLEW_VERSION_2_1) {
                return new GLDeviceConcrete<DV_OPENGL_2_1>();
            }
            return 0;
            
        case DV_OPENGL_3_0:
            if (GLEW_VERSION_3_0) {
                return new GLDeviceConcrete<DV_OPENGL_3_0>();
            }
            return 0;

        case DV_OPENGL_3_1:
            if (GLEW_VERSION_3_1) {
                return new GLDeviceConcrete<DV_OPENGL_3_1>();
            }
            return 0;

        case DV_OPENGL_3_2:
            if (GLEW_VERSION_3_2) {
                return new GLDeviceConcrete<DV_OPENGL_3_2>();
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
#endif // !defined(__ANDROID__)
}

// define scalar type traits
//const scalar_type_desc T::SCALAR_TYPE_TRAITS[]
