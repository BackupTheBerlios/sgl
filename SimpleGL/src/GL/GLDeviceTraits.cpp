#include "GL/GLDeviceTraits.h"
#include "GL/GLTexture2D.h"

using namespace sgl;

GLDeviceTraits::GLDeviceTraits(Device* _device) :
    device(_device)
{
#ifdef SIMPLE_GL_ES
    shaderModel = 200;
#else
    // determine the shader model checking different extensions
    // according to http://ogltotd.blogspot.com/2006/12/checking-for-d3d-shader-version.html
    if ( glewIsSupported("GL_EXT_gpu_shader4") ) {
        shaderModel = 400;
    }
    else if ( glewIsSupported("GL_NV_vertex_program3")
              || glewIsSupported("GL_ATI_shader_texture_lod") )
    {
        shaderModel = 300;
    }
    else if ( glewIsSupported("GL_ARB_fragment_program") ) {
        shaderModel = 200;
    }
    else if ( glewIsSupported("GL_ATI_fragment_shader") ) {
        shaderModel = 140;
    }
    else if ( glewIsSupported("GL_NV_texture_shader") ) {
        shaderModel = 110;
    }
    else if ( glewIsSupported("GL_ARB_vertex_program") ) {
        shaderModel = 100;
    }
    else {
        shaderModel = 0;
    }
#endif

	// extenions
    //glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, (GLint*)&numVertexTIU);
    //glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint*)&numCombinedTIU);

#ifndef SIMPLE_GL_ES
    // other extesions
    supportsSeparateStencil = ( glewIsSupported("GL_ATI_separate_stencil") != 0);
    supportsNPOT            = ( glewIsSupported("GL_ARB_texture_non_power_of_two") != 0);
    supportsHardwareMipmap  = ( glewIsSupported("GL_SGIS_generate_mipmap") != 0);
#endif
}
