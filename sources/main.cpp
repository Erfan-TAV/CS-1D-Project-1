#include <iostream>

#include "../headers/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFileInfo>

#include "databaseHelper.h"
#include "dbManager.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Initialize DbManager
    // const QString serverPath = QCoreApplication::applicationDirPath() + "/project1.db";
    DbManager db("C:/Users/erfan/Documents/CS1D project 1/cmake-build-debug/project1.db");



    qDebug() << closestCampus(1);

    // if (addCampus("UC Irv0000ine")) {
    //     qDebug() << "Success: Added UC Irvine";
    // }
    //
    // if (db.addSouvenir(15, "Blue & Gold Hoodie", 45.99)) {
    //     qDebug() << "Success: Added Hoodie to UC Irvine";
    // }
    // -------------------------------------
    w.setStyle(QStyleFactory::create("Fusion"));

    w.show();

    return QApplication::exec();
}
