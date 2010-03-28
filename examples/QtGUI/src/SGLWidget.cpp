#include "GL/GLCommon.h"
#include "SGLWidget.h"

SGLWidget::SGLWidget(const QGLFormat& format) :
    QGLWidget(format)
{
}

sgl::Device* SGLWidget::getDevice()
{
    if (!device)
    {
        device.reset( sglCreateDeviceFromCurrent(sgl::DV_OPENGL_2_0) );
        if (!device) {
            throw std::runtime_error("Can't acquire device");
        }

        // create default depth stencil state
        {
            sgl::DepthStencilState::DESC desc;
            desc.stencilEnable = false;
            desc.depthEnable   = true;
            desc.depthFunc     = sgl::DepthStencilState::LEQUAL;

            depthStencilState.reset( device->CreateDepthStencilState(desc) );
        }
    }

    return device.get();
}

void SGLWidget::setupQtStates()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void SGLWidget::setupSglStates()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    //flushVertexLayout->Unbind();
    depthStencilState->Bind();
}
