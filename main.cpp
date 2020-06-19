#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::addLibraryPath(QDir::currentPath());
    QCoreApplication::addLibraryPath(QDir::currentPath()+"/platforms");
    MainWindow w;
    w.show();
    return a.exec();
}
