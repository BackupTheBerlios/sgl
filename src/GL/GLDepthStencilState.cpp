#include "GL/GLDepthStencilState.h"
#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

namespace {

    using namespace sgl;

    GLenum BIND_COMPARISON_FUNCTION[] =
    {
	    GL_NEVER,
	    GL_LESS,
	    GL_LEQUAL,
	    GL_GREATER,
	    GL_GEQUAL,
	    GL_EQUAL,
	    GL_NOTEQUAL,
	    GL_ALWAYS
    };

    GLenum BIND_OPERATION[] =
    {
	    GL_KEEP,
	    GL_ZERO,
	    GL_REPLACE,
	    GL_INCR,
	    GL_DECR,
	    GL_INCR_WRAP,
	    GL_DECR_WRAP,
	    GL_INVERT,
    };

    template<DEVICE_VERSION DeviceVersion>
    class GLDepthStencilStateDisplayLists :
        public ReferencedImpl<DepthStencilState>
    {
    public:
        GLDepthStencilStateDisplayLists(GLDevice<DeviceVersion>* device_, const DESC& desc_) :
            device(device_),
            desc(desc_)
        {
            bindDisplayList = glGenLists(1);

            // generate setup list
            glNewList(bindDisplayList, GL_COMPILE);
            {
                if (desc.depthEnable)
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(BIND_COMPARISON_FUNCTION[desc.depthFunc]);
                }
                else {
                    glDisable(GL_DEPTH_TEST);
                }
                glDepthMask(desc.depthWriteMask);

                if (desc.stencilEnable)
                {
		            glEnable(GL_STENCIL_TEST);
                    glStencilMask(desc.stencilWriteMask);

                    glStencilFuncSeparate(GL_FRONT, BIND_COMPARISON_FUNCTION[desc.frontFaceOp.stencilFunc], 0, desc.stencilReadMask);
                    glStencilFuncSeparate(GL_BACK,  BIND_COMPARISON_FUNCTION[desc.backFaceOp.stencilFunc],  0, desc.stencilReadMask);

                    glStencilOpSeparate( GL_FRONT, 
                                         BIND_OPERATION[desc.frontFaceOp.stencilFailOp], 
                                         BIND_OPERATION[desc.frontFaceOp.stencilDepthFailOp],
                                         BIND_OPERATION[desc.frontFaceOp.stencilPassOp] );
                    glStencilOpSeparate( GL_BACK, 
                                         BIND_OPERATION[desc.backFaceOp.stencilFailOp], 
                                         BIND_OPERATION[desc.backFaceOp.stencilDepthFailOp],
                                         BIND_OPERATION[desc.backFaceOp.stencilPassOp] );
                }
                else {
                    glDisable(GL_STENCIL_TEST);
                }
            }
            glEndList();
        }

        ~GLDepthStencilStateDisplayLists()
        {
            glDeleteLists(bindDisplayList, 1);
            if (device->CurrentDepthStencilState() == this) {
                device->SetDepthStencilState(0);
            }
        }

        // Override State
        void SGL_DLLCALL Bind() const
        {
            glCallList(bindDisplayList);
            device->SetDepthStencilState(this);
        }

        // Override RasterizerState
        const DESC& SGL_DLLCALL Desc() const { return desc; }

    private:
        GLDevice<DeviceVersion>*    device;

        // setup display list
        GLuint                      bindDisplayList;

        // properties
        DESC                        desc;
    };

    template<DEVICE_VERSION DeviceVersion>
    class GLDepthStencilStateSeparate :
        public ReferencedImpl<DepthStencilState>
    {
    public:
        GLDepthStencilStateSeparate(GLDevice<DeviceVersion>* device_, const DESC& desc_) :
            device(device_),
            desc(desc_)
        {
        }

        ~GLDepthStencilStateSeparate()
        {
            if (device->CurrentDepthStencilState() == this) {
                device->SetDepthStencilState(0);
            }
        }

        // Override State
        void SGL_DLLCALL Bind() const
        {
            if (desc.depthEnable)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(BIND_COMPARISON_FUNCTION[desc.depthFunc]);
            }
            else {
                glDisable(GL_DEPTH_TEST);
            }
            glDepthMask(desc.depthWriteMask);

            if (desc.stencilEnable)
            {
                glEnable(GL_STENCIL_TEST);
                glStencilMask(desc.stencilWriteMask);

                glStencilFuncSeparate(GL_FRONT, BIND_COMPARISON_FUNCTION[desc.frontFaceOp.stencilFunc], 0, desc.stencilReadMask);
                glStencilFuncSeparate(GL_BACK,  BIND_COMPARISON_FUNCTION[desc.backFaceOp.stencilFunc],  0, desc.stencilReadMask);

                glStencilOpSeparate( GL_FRONT,
                                     BIND_OPERATION[desc.frontFaceOp.stencilFailOp],
                                     BIND_OPERATION[desc.frontFaceOp.stencilDepthFailOp],
                                     BIND_OPERATION[desc.frontFaceOp.stencilPassOp] );
                glStencilOpSeparate( GL_BACK,
                                     BIND_OPERATION[desc.backFaceOp.stencilFailOp],
                                     BIND_OPERATION[desc.backFaceOp.stencilDepthFailOp],
                                     BIND_OPERATION[desc.backFaceOp.stencilPassOp] );
            }
            else {
                glDisable(GL_STENCIL_TEST);
            }

            device->SetDepthStencilState(this);
        }

        // Override RasterizerState
        const DESC& SGL_DLLCALL Desc() const { return desc; }

    private:
        GLDevice<DeviceVersion>*    device;
        DESC                        desc;
    };

} // anonymous namespace

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
sgl::DepthStencilState* sglCreateDepthStencilState(GLDevice<DeviceVersion>* device, const DepthStencilState::DESC& desc)
{
    if ( device_traits<DeviceVersion>::support_display_lists() ) {
        return new GLDepthStencilStateDisplayLists<DeviceVersion>(device, desc);
    }
    else {
        return new GLDepthStencilStateSeparate<DeviceVersion>(device, desc);
    }
}

// explicit template instantiation
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_1_3>(GLDevice<DV_OPENGL_1_3>*, const DepthStencilState::DESC&);
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_1_4>(GLDevice<DV_OPENGL_1_4>*, const DepthStencilState::DESC&);
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_1_5>(GLDevice<DV_OPENGL_1_5>*, const DepthStencilState::DESC&);
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_2_0>(GLDevice<DV_OPENGL_2_0>*, const DepthStencilState::DESC&);
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_2_1>(GLDevice<DV_OPENGL_2_1>*, const DepthStencilState::DESC&);
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_3_0>(GLDevice<DV_OPENGL_3_0>*, const DepthStencilState::DESC&);
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_3_1>(GLDevice<DV_OPENGL_3_1>*, const DepthStencilState::DESC&);
template sgl::DepthStencilState* sglCreateDepthStencilState<DV_OPENGL_3_2>(GLDevice<DV_OPENGL_3_2>*, const DepthStencilState::DESC&);

} // namespace sgl
