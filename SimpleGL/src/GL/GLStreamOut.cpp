#include "GL/GLStreamOut.h"

using namespace sgl;

GLStreamOut::GLStreamOut(GLProgram* _program) :
    program(_program),
    dirty(true),
    streamOutMode(INTERLEAVED),
    primitiveType(TRIANGLES)
{
}

GLStreamOut::~GLStreamOut()
{
    if (feedbackVaryings) 
    {
        for(size_t i = 0; i<declaration.size(); ++i) {
            free(feedbackVaryings[i]);
        }
        free(feedbackVaryings);
    }
}

SGL_HRESULT SGL_DLLCALL GLStreamOut::SetPrimitiveType(PRIMITIVE_TYPE _primitiveType)
{
    #ifndef SGL_NO_STATUS_CHECK
    if ( _primitiveType != POINTS
         && _primitiveType != LINES
         && _primitiveType != TRIANGLES ) 
    {
        return EInvalidCall("GLStreamOut::SetPrimitiveType failed. Primitive type is not POINTS, LINES or TRIANGLES");
    }
    #endif

    primitiveType = _primitiveType;
    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL GLStreamOut::SetStreamOutDecl(unsigned int numEntries, DeclarationEntry* soDecl)
{
    // copy declaration
    declaration.resize(numEntries);
    std::copy( soDecl, soDecl + numEntries, declaration.begin() );

    // make utility structures
    if (feedbackVaryings) 
    {
        for(size_t i = 0; i<declaration.size(); ++i) {
            free(feedbackVaryings[i]);
        }
        free(feedbackVaryings);
    }   
    feedbackVaryings = (char**)malloc( declaration.size() * sizeof(char*) );
    

    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL GLStreamOut::SetTargets(unsigned int numEntries, VertexBuffer** vbos)
{
    // check
    for(unsigned int i = 0; i<numEntries; ++i) 
    {
        if (!vbos[i]) {
            return EInvalidCall("GLStreamOut::SetTargets failed. Vertex buffer target can't be 0");
        }
    }

    // copy
    targets.resize(numEntries);
    for(unsigned int i = 0; i<numEntries; ++i) {
        targets[i].reset( static_cast<GLVertexBuffer*>(vbos[i]) );
    }

    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL GLStreamOut::Dirty(bool force)
{
    if (!force && !dirty) {
        return SGL_OK;
    }

    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL GLStreamOut::Bind() const
{
    return SGL_OK;
}

void SGL_DLLCALL GLStreamOut::Unbind() const
{
}
