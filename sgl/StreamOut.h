#ifndef SIMPLE_GL_STREAM_OUT_H
#define SIMPLE_GL_STREAM_OUT_H

#include "VertexBuffer.h"
#include "Program.h"

namespace sgl {

/** Interface for defining stream output stage(TransformFeedback in the OpenGL).
 * It is attached to the shader program.
 */
class StreamOut : 
    public Referenced
{
public:
    enum MODE
    {
        SEPARATE, /// Stream out vertices to one or more buffers. Each declaration entry
                  /// will be stored in the buffer with index corresponding declaration.
        INTERLEAVED /// Stream out vertices to single buffer one by one.
    };

    /** Single stream output element declaration */
    struct DeclarationEntry
    {
        const char*     semantic; /// Could be POSITION, NORMAL, TEXCOORDi or name of the
                                  /// openGL varying. You can't mix varying semantic
                                  /// with standart API semantic in one declaration.
        unsigned char   componentCount; /// number of components for streamin out. If the semantic
                                        /// is name of the varying variable this ignored.
    };

public:
    /** Get program to wich stream out is attached */
    virtual Program* SGL_DLLCALL MasterProgram() const = 0;

    /** Setup stream out mode. Default is INTERLEAVED.
     * @param streamOutMode - SEPARATE ot INTERLEAVED mode 
     */
    virtual void SGL_DLLCALL SetMode(MODE streamOutMode) = 0;

    /** Get stream output mode */
    virtual MODE SGL_DLLCALL Mode() const = 0;

    /** Setup feedback primitive type. Default is TRIANGLES
     * @param primitiveType - primitive type of the output stream
     * @return result of the operation. Could be SGLERR_INVALID_CALL id the primitive type
     * is not the one of the POINTS, LINES, TRIANGLES.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetPrimitiveType(PRIMITIVE_TYPE primitiveType) = 0;

    /** Get feedback primitive type */
    virtual PRIMITIVE_TYPE SGL_DLLCALL PrimitiveType() const = 0;

    /** Set stream output declaration
     * @param numEntried - number of the entries in the soDecl array
     * @param soDecl - array of the stream output declaration entries
     * @return result of the operation. Could be SGLERR_INVALID call if the declaration is invalid,
     * SGLERR_UNSUPPORTED if device doesn't support such declaration.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetStreamOutDecl(unsigned int numEntries, DeclarationEntry* soDecl) = 0;

    /** Get number of the stream output declaration entries */
    virtual unsigned int SGL_DLLCALL NumDeclEntries() const = 0;

    /** Get i'th stream output declaration entry */
    virtual DeclarationEntry SGL_DLLCALL DeclEntry(unsigned int index) const = 0;

    /** Set stream output vertex buffer targets 
     * @param numEntries - number of the buffers in the vbos array
     * @param vbos - array of the vertex buffers
     * @return result of the operation. Could be SGLERR_INVALID_CALL if one of the vbos is 0, 
     * SGLERR_UNSUPPORTED if the offset is invalid for current device.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetTargets(unsigned int numEntries, VertexBuffer** vbos) = 0;

    /** Get number of the stream output buffer targets */
    virtual unsigned int SGL_DLLCALL NumTargets() const = 0;

    /** Get i'th stream output buffer target */
    virtual VertexBuffer* SGL_DLLCALL Target(unsigned int index) const = 0;

    /** Try to compile the resource 
     * @param force - force recreation even if the resource is initialized.
     * @return result of the operation. Could be SGLERR_INVALID_CALL - if the combination of the parameters is invalid,
     * SGLERR_UNSUPPORTED - if the device doesn't support such stream output configuration.  
     */
    virtual SGL_HRESULT SGL_DLLCALL Dirty(bool force = false) = 0;

    virtual ~StreamOut() {}
};

} // namespace sgl

#endif // SIMPLE_GL_STREAM_OUT_H
