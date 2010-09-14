#include "GL/GLRasterizerState.h"
#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

namespace {

    using namespace sgl;

    GLenum BIND_CULL_MODE[] =
    {
        GL_NONE,
        GL_BACK,
        GL_FRONT,
	    GL_FRONT_AND_BACK
    };

    GLenum BIND_FILL_MODE[] =
    {
	    GL_FILL,
	    GL_LINE
    };

    template<DEVICE_VERSION DeviceVersion>
    class GLRasterizerStateDefault :
        public ReferencedImpl<RasterizerState>
    {
    public:
        GLRasterizerStateDefault(GLDevice<DeviceVersion>* device_, const DESC& desc_) :
            device(device_),
            desc(desc_)
        {
            glCullMode  = BIND_CULL_MODE[desc.cullMode];
            glFillMode  = BIND_FILL_MODE[desc.fillMode];
        }

        ~GLRasterizerStateDefault()
        {
            if (device->CurrentRasterizerState() == this) {
                device->SetRasterizerState(0);
            }
        }

        // Override RasterizerState
        const DESC& SGL_DLLCALL Desc() const 
        { 
            return desc; 
        }

        void SGL_DLLCALL Bind() const
        {
            if (desc.cullMode != NONE)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(glCullMode);
            }
            else {
                glDisable(GL_CULL_FACE);
            }
            glPolygonMode(GL_FRONT_AND_BACK, glFillMode);
            glColorMask(desc.colorMask & RED,
                        desc.colorMask & GREEN,
                        desc.colorMask & BLUE,
                        desc.colorMask & ALPHA);

            device->SetRasterizerState(this);
        }

    private:
        GLDevice<DeviceVersion>*    device;
        DESC                        desc;

        // gl primitives
        GLuint      glCullMode;
        GLuint      glFillMode;
    };

} // anonymous namespace

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
sgl::RasterizerState* sglCreateRasterizerState(GLDevice<DeviceVersion>* device, const RasterizerState::DESC& desc)
{
    return new GLRasterizerStateDefault<DeviceVersion>(device, desc);
}

// explicit template instantiation
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_1_3>(GLDevice<DV_OPENGL_1_3>*, const RasterizerState::DESC&);
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_1_4>(GLDevice<DV_OPENGL_1_4>*, const RasterizerState::DESC&);
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_1_5>(GLDevice<DV_OPENGL_1_5>*, const RasterizerState::DESC&);
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_2_0>(GLDevice<DV_OPENGL_2_0>*, const RasterizerState::DESC&);
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_2_1>(GLDevice<DV_OPENGL_2_1>*, const RasterizerState::DESC&);
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_3_0>(GLDevice<DV_OPENGL_3_0>*, const RasterizerState::DESC&);
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_3_1>(GLDevice<DV_OPENGL_3_1>*, const RasterizerState::DESC&);
template sgl::RasterizerState* sglCreateRasterizerState<DV_OPENGL_3_2>(GLDevice<DV_OPENGL_3_2>*, const RasterizerState::DESC&);

} // namespace sgl
