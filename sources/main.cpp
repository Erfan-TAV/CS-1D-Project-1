#include "../headers/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

#include "databaseHelper.h"
#include "dbManager.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Initialize DbManager
    DbManager db("project1.db");

    if (addCampus("UC Irv0000ine")) {
        qDebug() << "Success: Added UC Irvine";
    }
    //
    // if (db.addSouvenir(15, "Blue & Gold Hoodie", 45.99)) {
    //     qDebug() << "Success: Added Hoodie to UC Irvine";
    // }
    // -------------------------------------
    w.setStyle(QStyleFactory::create("Fusion"));

    w.show();

    return QApplication::exec();
}
