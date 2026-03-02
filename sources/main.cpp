#include "../headers/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include "databaseHelper.h"
#include "dbManager.h"
#include "testmainwindow.h"
#include "adminpage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    TestMainWindow admin;

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

    addCampus("test campus");
    qDebug() << closestCampus(1);

    // 4. Initialize your DbManager with the dynamic path
    w.setStyle(QStyleFactory::create("Fusion"));

    w.show();
    // admin.show();



    return QApplication::exec();
}
