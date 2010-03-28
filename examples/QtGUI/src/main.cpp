#include <QDialog>
#include <QtGui>
#include "Scene.h"
#include "SGLWidget.h"
#include "GraphicsView.h"

int main(int argc, char **argv)
{
    std::vector<int> ints;
    ints.push_back(3);
    ints.push_back(5);

    std::string value = "asdasdas";

    std::vector<std::string> strs;
    strs.push_back("abv");
    strs.push_back("abc");

    math::Vector4f vec(0.4f, 0.3f, 1.0f, 1.0f);

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
