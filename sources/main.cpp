#include <iostream>

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
    MainWindow w;

    a.setOrganizationName("MyProjectGroup");
    a.setApplicationName("CampusProject");

    // 1. Get the target path
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appDataPath + "/project1.db";
    //
    // qDebug() << "--- Database Debug Info ---";
    // qDebug() << "Expected Folder:" << appDataPath;
    // qDebug() << "Full DB Path:   " << dbPath;

    // Initialize DbManager
    DbManager db(dbPath);

    if (!db.isOpen()) {
        qDebug() << "Failed to initialize database.";
        return -1;
    }

    // 4. Initialize your DbManager with the dynamic path
    w.setStyle(QStyleFactory::create("Fusion"));

    w.show();

    return QApplication::exec();
}
