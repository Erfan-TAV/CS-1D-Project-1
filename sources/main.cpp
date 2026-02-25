#include <iostream>

#include "../headers/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFileInfo>
#include <Qdir>
#include <QStandardPaths>

#include "databaseHelper.h"
#include "dbManager.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    a.setOrganizationName("MyProjectGroup");
    a.setApplicationName("CampusProject");

    // 1. Get the target path
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appDataPath + "/project1.db";

    qDebug() << "--- Database Debug Info ---";
    qDebug() << "Expected Folder:" << appDataPath;
    qDebug() << "Full DB Path:   " << dbPath;
    qDebug() << "Resource Exist? " << QFile::exists(":/project1.db");
    qDebug() << "Target Exist?   " << QFile::exists(dbPath);

    // Initialize DbManager
    // QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath("."); // Create folder if it doesn't exist
        qDebug() << "!dir.exists";
    }

    // QString dbPath = appDataPath + "/project1.db";

    // 2. If it's not there, copy it from our Resources (: prefix)
    QFile dbFile(dbPath);
    if (!dbFile.exists() || dbFile.size() == 0) {
        if (dbFile.exists()) {
            dbFile.remove(); // Delete the "blank" one so we can copy the real one
        }

        // Ensure directory exists
        QDir().mkpath(appDataPath);

        // Copy from resources (prefixed with :)
        if (QFile::copy(":/project1.db", dbPath)) {
            // Fix permissions: macOS often marks copied resources as Read-Only
            QFile::setPermissions(dbPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
            qDebug() << "Template database successfully copied to AppData.";
        } else {
            qDebug() << "Failed to copy! Check if project1.db is in resources.qrc";
        }
    }

    // 3. Initialize your DbManager with the dynamic path
    DbManager db(dbPath);

    qDebug() << closestCampus(1);

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
