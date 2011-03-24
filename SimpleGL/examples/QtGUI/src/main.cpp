#include <QDialog>
#include <QtGui>
#include "Scene.h"
#include "SGLWidget.h"
#include "GraphicsView.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    try
    {
        GraphicsView view;
        SGLWidget   sglWidget( QGLFormat(QGL::SampleBuffers) );

        view.setViewport(&sglWidget);
        view.setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
        view.setScene( new Scene(&sglWidget) );
        view.show();
        view.resize(1024, 768);

        return app.exec();
    }
    catch(std::exception& ex)
    {
        std::cout << "Exeption caught: " << ex.what() << std::endl
                  << "Exiting";
        return EXIT_FAILURE;
    }
}
