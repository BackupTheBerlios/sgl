#ifndef SIMPLE_GL_FX_SHADER_UTILITY_H
#define SIMPLE_GL_FX_SHADER_UTILITY_H

#include "../../Device.h"
#include <cstdio>
#include <string>
#ifdef __ANDROID__
#   include <android/asset_manager.h>
#endif

namespace sgl {

inline Shader* CreateShaderFromFile( Device*            device,
                                     Shader::TYPE       type,
                                     const char*        fileName
                                     #ifdef __ANDROID__
                                     ,  AAssetManager*  mgr = 0
                                     #endif
                                     )
{
    std::string source;
#ifdef __ANDROID__
    if (mgr)
    {
        AAsset* asset  = AAssetManager_open(mgr, fileName, AASSET_MODE_STREAMING);
        if (!asset)
        {
            sglSetError(SGLERR_FILE_NOT_FOUND, (std::string("CreateShaderFromFile failed. Can't open shader asset: ") + fileName).c_str() );
            return 0;
        }

        off_t length = AAsset_getLength(asset);
        source.resize(length);
        size_t read = AAsset_read(asset, &source[0], length);
        assert(read == length);
    }
    else
    {
#endif
        // files just for speed
        FILE* file = fopen(fileName, "r");
        if (!file)
        {
            sglSetError(SGLERR_FILE_NOT_FOUND, (std::string("CreateShaderFromFile failed. Can't open shader file: ") + fileName).c_str() );
            return 0;
        }

        // insert defines
        {
            fseek(file, 0, SEEK_END);
            long size = ftell(file);
            source.resize(size);
            fseek(file, 0, SEEK_SET);
            size_t numRead = fread(&source[0], 1, size, file);
            source.resize(numRead);
            fclose(file);
        }
#ifdef __ANDROID__
    }
#endif

    Shader::DESC desc;
    desc.type   = type;
    desc.source = source.c_str();
    return device->CreateShader(desc);
}

} // namespace sgl

#endif // SIMPLE_GL_FX_SHADER_UTILITY_H
