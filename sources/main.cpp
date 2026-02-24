#include "../headers/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setStyle(QStyleFactory::create("Fusion"));
    w.show();
    return a.exec();
}
