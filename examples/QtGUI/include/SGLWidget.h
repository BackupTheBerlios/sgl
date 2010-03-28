#ifndef SIMPLE_GL_QT_GUI_SGL_WIDGET_H
#define SIMPLE_GL_QT_GUI_SGL_WIDGET_H

#include <QGLWidget>
#include "Device.h"

class SGLWidget :
    public QGLWidget
{
public:
    explicit SGLWidget(const QGLFormat& format);

    /** Setup states for qt rendering */
    void setupQtStates();

    /** Setup states for sgl rendering */
    void setupSglStates();

    /** Get sgl device */
    sgl::Device* getDevice();

private:
    sgl::ref_ptr<sgl::Device>               device;
    sgl::ref_ptr<sgl::DepthStencilState>    depthStencilState;
    sgl::ref_ptr<sgl::VertexLayout>         flushVertexLayout;
};

#endif // SIMPLE_GL_QT_GUI_SGL_WIDGET_H
