/**
 * @file main.cpp
 * @brief Application entry point and initialization of database. 
 */

#include "../headers/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include "dbManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("MyProjectGroup");
    a.setApplicationName("CampusProject");

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appDataPath + "/project1.db";

    // Initialize DbManager
    DbManager db(dbPath);

    if (!db.isOpen()) {
        qDebug() << "Failed to initialize database.";
        return -1;
    }

    // addCampus("test campus");
    // qDebug() << closestCampus(1);

    MainWindow w;

    // 4. Initialize your DbManager with the dynamic path
    // w.setStyle(QStyleFactory::create("Fusion"));

    w.show();
    // admin.show();



    return QApplication::exec();
}
