#include <QApplication>
#include "MainWindow.hpp"
#include "ChartWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    MainWindow window;
    window.show();

    return app.exec();
}
