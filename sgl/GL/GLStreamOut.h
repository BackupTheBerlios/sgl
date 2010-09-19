#ifndef SIMPLE_GL_GL_STREAM_OUT_H
#define SIMPLE_GL_GL_STREAM_OUT_H

#include "../StreamOut.h"
#include "GLVertexBuffer.h"
#include "GLProgram.h"
#include <vector>

namespace sgl {

/** OpenGL stream output implementation via TranformFeedback */
class GLStreamOut : 
    public StreamOut
{
private:
    typedef std::vector<DeclarationEntry>           declaration_vector;
    typedef std::vector< ref_ptr<GLVertexBuffer> >  target_vector;

public:
    GLStreamOut(GLProgram* _program);
    ~GLStreamOut();

    // Override StreamOut
    Program* SGL_DLLCALL MasterProgram() const { return program; }

    void SGL_DLLCALL SetMode(MODE _streamOutMode) { streamOutMode = _streamOutMode; }
    MODE SGL_DLLCALL Mode() const { return streamOutMode; }

    SGL_HRESULT SGL_DLLCALL SetPrimitiveType(PRIMITIVE_TYPE primitiveType);
    PRIMITIVE_TYPE SGL_DLLCALL PrimitiveType() const { return primitiveType; }
   
    SGL_HRESULT         SGL_DLLCALL SetStreamOutDecl(unsigned int numEntries, DeclarationEntry* soDecl);
    unsigned int        SGL_DLLCALL NumDeclEntries() const { return declaration.size(); }
    DeclarationEntry    SGL_DLLCALL DeclEntry(unsigned int index) const { return declaration[index]; }
    
    SGL_HRESULT     SGL_DLLCALL SetTargets(unsigned int numEntries, VertexBuffer** vbos);
    unsigned int    SGL_DLLCALL NumTargets() const { return targets.size(); }
    VertexBuffer*   SGL_DLLCALL Target(unsigned int index) const { return targets[index].get(); }

    SGL_HRESULT SGL_DLLCALL Dirty(bool force = false);

    /** Bind stream output stage 
     * @return result of the operation. Same as in Dirty.
     * @see Dirty
     */
    SGL_HRESULT SGL_DLLCALL Bind() const;

    /** Unbind stream output stage */
    void SGL_DLLCALL Unbind() const;

private:
    GLProgram*          program;

    // settings
    bool                dirty;
    MODE                streamOutMode;
    PRIMITIVE_TYPE      primitiveType;
    declaration_vector  declaration;
    target_vector       targets;

    // openGL
    char**              feedbackVaryings;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_STREAM_OUT_H
