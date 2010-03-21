#include "GL/GLBlendState.h"
#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

namespace {

    using namespace sgl;

    GLenum BIND_BLEND_OPERATION[] =
    {
        GL_FUNC_ADD,
        GL_FUNC_SUBTRACT,
        GL_FUNC_REVERSE_SUBTRACT,
        GL_MIN,
        GL_MAX
    };

    GLenum BIND_BLEND_FUNCTION[] =
    {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA,
        GL_SRC_ALPHA_SATURATE
    };

    template<DEVICE_VERSION deviceVersion>
    class GLBlendStateDisplayLists :
        public ReferencedImpl<BlendState>
    {
    public:
        GLBlendStateDisplayLists(GLDevice<deviceVersion>*       device_, 
                                 const BlendState::DESC&        desc_) :
            device(device_),
            desc(desc_)
        {
            bindDisplayList = glGenLists(1);

            // generate setup list
            glNewList(bindDisplayList, GL_COMPILE);
            {
                if (desc.blendEnable)
                {
                    glEnable(GL_BLEND);
                    if (desc.srcBlend != desc.srcBlendAlpha || desc.destBlend != desc.destBlendAlpha)
                    {
                        glBlendFuncSeparate( BIND_BLEND_FUNCTION[desc.srcBlend], 
                                             BIND_BLEND_FUNCTION[desc.destBlend],
                                             BIND_BLEND_FUNCTION[desc.srcBlendAlpha],
                                             BIND_BLEND_FUNCTION[desc.destBlendAlpha] );
                    }
                    else 
                    {
                        glBlendFunc(BIND_BLEND_FUNCTION[desc.srcBlend], BIND_BLEND_FUNCTION[desc.destBlend]);
                    }

                    if (desc.blendOp != desc.blendOpAlpha) {
                        glBlendEquationSeparate(BIND_BLEND_OPERATION[desc.blendOp], BIND_BLEND_OPERATION[desc.blendOpAlpha]);
                    }
                }
                else {
                    glDisable(GL_BLEND);
                }
            }
            glEndList();
        }
            
        ~GLBlendStateDisplayLists()
        {
            glDeleteLists(bindDisplayList, 1);
        }

        void SGL_DLLCALL Bind() const
        {
            if (device->CurrentBlendState() != this) 
            {
                glCallList(bindDisplayList);
                device->SetBlendState(this);
            }
        }
        
        const BlendState::DESC& SGL_DLLCALL Desc() const { return desc; }

    private:
        GLDevice<deviceVersion>*        device;
        GLuint                          bindDisplayList;
        BlendState::DESC                desc;
    };

    template<DEVICE_VERSION deviceVersion>
    class GLBlendStateSeparate :
        public ReferencedImpl<BlendState>
    {
    public:
        GLBlendStateSeparate(GLDevice<deviceVersion>*       device_, 
                             const BlendState::DESC&        desc_) :
            device(device_),
            desc(desc_)
        {
        }

        void SGL_DLLCALL Bind() const
        {
            if (device->CurrentBlendState() != this) 
            {
                if (desc.blendEnable)
                {
                    glEnable(GL_BLEND);
                    glBlendFuncSeparate( BIND_BLEND_FUNCTION[desc.srcBlend], 
                                         BIND_BLEND_FUNCTION[desc.destBlend],
                                         BIND_BLEND_FUNCTION[desc.srcBlendAlpha],
                                         BIND_BLEND_FUNCTION[desc.destBlendAlpha] );
                    glBlendEquationSeparate( BIND_BLEND_OPERATION[desc.blendOp], 
                                             BIND_BLEND_OPERATION[desc.blendOpAlpha] );
                }
                else {
                    glDisable(GL_BLEND);
                }

                device->SetBlendState(this);
            }
        }
        
        const BlendState::DESC& SGL_DLLCALL Desc() const { return desc; }

    private:
        GLDevice<deviceVersion>*        device;
        BlendState::DESC                desc;
    };

    template<DEVICE_VERSION deviceVersion>
    class GLBlendStateDefault :
        public ReferencedImpl<BlendState>
    {
    public:
        GLBlendStateDefault(GLDevice<deviceVersion>*   device_, 
                            const BlendState::DESC&    desc_) :
            device(device_),
            desc(desc_)
        {
            glSrcBlend  = BIND_BLEND_FUNCTION[desc.srcBlend];
            glDestBlend = BIND_BLEND_FUNCTION[desc.destBlend];
            glBlendOp   = BIND_BLEND_OPERATION[desc.blendOp];
        }

        void SGL_DLLCALL Bind() const
        {
            if (device->CurrentBlendState() != this) 
            {
                if (desc.blendEnable)
                {
                    glEnable(GL_BLEND);
                    glBlendFunc(glSrcBlend, glDestBlend);
                    glBlendEquation(glBlendOp);
                }
                else {
                    glDisable(GL_BLEND);
                }

                device->SetBlendState(this);
            }
        }
        
        const BlendState::DESC& SGL_DLLCALL Desc() const { return desc; }

    private:
        GLDevice<deviceVersion>*    device;
        BlendState::DESC            desc;

        // gl bindings
        GLuint  glSrcBlend;
        GLuint  glDestBlend;
        GLuint  glBlendOp;
    };

} // anonymous namespace

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
sgl::BlendState* sglCreateBlendState(GLDevice<DeviceVersion>* device, const BlendState::DESC& desc)
{
    if ( desc.destBlend != desc.destBlendAlpha 
         || desc.srcBlend != desc.srcBlendAlpha
         || desc.blendOp != desc.blendOpAlpha )
    {
        if (!GL_ARB_draw_buffers_blend) {
            throw gl_error("Separate blending is not supported", SGLERR_UNSUPPORTED);
        }

        if ( device_traits<DeviceVersion>::support_display_lists() ) {
            return new GLBlendStateDisplayLists<DeviceVersion>(device, desc);
        }
        else {
            return new GLBlendStateSeparate<DeviceVersion>(device, desc);
        }
    }
    else {
        return new GLBlendStateDefault<DeviceVersion>(device, desc);
    }
}

// explicit template instantiation
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_1_3>(GLDevice<DV_OPENGL_1_3>*, const BlendState::DESC&);
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_1_4>(GLDevice<DV_OPENGL_1_4>*, const BlendState::DESC&);
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_1_5>(GLDevice<DV_OPENGL_1_5>*, const BlendState::DESC&);
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_2_0>(GLDevice<DV_OPENGL_2_0>*, const BlendState::DESC&);
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_2_1>(GLDevice<DV_OPENGL_2_1>*, const BlendState::DESC&);
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_3_0>(GLDevice<DV_OPENGL_3_0>*, const BlendState::DESC&);
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_3_1>(GLDevice<DV_OPENGL_3_1>*, const BlendState::DESC&);
template sgl::BlendState* sglCreateBlendState<DV_OPENGL_3_2>(GLDevice<DV_OPENGL_3_2>*, const BlendState::DESC&);

} // namespace sgl