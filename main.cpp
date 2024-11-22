#include "windowfactory.h"
#include "iwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
   // w.show();
    IWindow *window = WindowFactory::createWindow("MainWindow");
    window->initialize();
    window->showWindow();
    return a.exec();
}
