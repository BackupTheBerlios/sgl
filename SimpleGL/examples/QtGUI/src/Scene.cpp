#include "Math/MatrixFunctions.hpp"
#include "Scene.h"
#include <QDialog>
#include <QGraphicsItem>
#include <QLabel>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPaintEngine>
#include <QTimer>
#include <QVBoxLayout>
#include "ui_settings_form.h"

Scene::Scene(SGLWidget* sglWidget_) :
    sglWidget(sglWidget_),
    initialized(false)
{
    QDialog* settingsForm = createDialog("Settigns");
    {
        Ui::SettingsForm ui;
        ui.setupUi(settingsForm);
    }
    addWidget(settingsForm);

    QPointF pos(10, 10);
    foreach (QGraphicsItem *item, items())
    {
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

        const QRectF rect = item->boundingRect();
        item->setPos(pos.x() - rect.x(), pos.y() - rect.y());
        pos += QPointF(0, 10 + rect.height());
    }
}

QDialog* Scene::createDialog(const QString &windowTitle) const
{
    QDialog *dialog = new QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    dialog->setWindowOpacity(0.8);
    dialog->setWindowTitle(windowTitle);
    dialog->setLayout(new QVBoxLayout);

    return dialog;
}

void Scene::init()
{
    // setup device options
    sgl::Device* device = sglWidget->getDevice();
    {
        device->SetClearColor( math::Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );

        guiCube = QSharedPointer<GUICube>( new GUICube(device) );

        // acquire params
        sgl::FFPProgram* ffpProgram = device->FixedPipelineProgram();
        projectionMatrixUniform     = ffpProgram->GetProjectionMatrixUniform();
        modelviewMatrixUniform      = ffpProgram->GetModelViewMatrixUniform();

        // create wireframe state
        {
            sgl::RasterizerState::DESC desc;
            desc.cullMode = sgl::RasterizerState::NONE;
            desc.fillMode = sgl::RasterizerState::WIREFRAME;

            wireframeState.reset( device->CreateRasterizerState(desc) );
        }
    }

    initialized = true;
}

void Scene::drawBackground(QPainter*, const QRectF &rect)
{
    if (!initialized) {
        init();
    }

    sglWidget->setupSglStates();
    {
        sgl::Device* device = sglWidget->getDevice();
        device->SetClearColor( math::Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
        device->SetViewport( sgl::rectangle(0, 0, rect.width(), rect.height()) );
        device->Clear(true, true, true);

        // bind states
        //wireframeState->Bind();

        projectionMatrixUniform->Set( math::make_perspective( 0.7853982f,
                                                              float(rect.width() / rect.height()),
                                                              0.1f,
                                                              100.0f ) );

        //projectionMatrixUniform->Set( math::make_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f) );
        modelviewMatrixUniform->Set( math::make_translation(0.0f, 0.0f, 5.0f) );

        guiCube->render(device);
    }
    sglWidget->setupQtStates();
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseMoveEvent(event);
    if (event->isAccepted()) {
        return;
    }

    event->accept();
    update();
}
