#include <QApplication>
#include "ChartWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ChartWindow window;
    window.show();

    return app.exec();
}
