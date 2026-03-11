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

    DbManager db(dbPath);

    if (!db.isOpen()) {
        qDebug() << "Failed to initialize database.";
        return -1;
    }

    MainWindow w;

    w.show();

    return QApplication::exec();
}
