#ifndef SIMPLE_GL_FX_SHADER_UTILITY_H
#define SIMPLE_GL_FX_SHADER_UTILITY_H

#include "../../Device.h"
#include <cstdio>
#include <string>

namespace sgl {

inline Shader* CreateShaderFromFile( Device*        device, 
                                     Shader::TYPE   type,
                                     const char*    fileName)
{
    // files just for speed
    FILE* file = fopen(fileName, "r");
    if (!file) 
    {
        sglSetError(SGLERR_FILE_NOT_FOUND, (std::string("CreateShaderFromFile failed. Can't open shader file: ") + fileName).c_str() );
        return 0;
    }

    // insert defines
    std::string source;
    {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        source.resize(size);
        fseek(file, 0, SEEK_SET);
        size_t numRead = fread(&source[0], 1, size, file);
        source.resize(numRead);
        fclose(file);
    }

    Shader::DESC desc;
    desc.type   = type;
    desc.source = source.c_str();
    return device->CreateShader(desc);
}

} // namespace sgl

#endif // SIMPLE_GL_FX_SHADER_UTILITY_H