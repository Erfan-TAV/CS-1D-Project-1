#include "dbManager.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QSqlError>
#include <QDirIterator>

DbManager::DbManager(const QString& fullPath) {
    // 1. Get the directory part of the path
    QFileInfo dbFileInfo(fullPath);
    QDir dir = dbFileInfo.absoluteDir();

    // 1. Get the target path
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appDataPath + "/project1.db";

    qDebug() << "--- Database Debug Info ---";
    qDebug() << "Expected Folder:" << appDataPath;
    qDebug() << "Full DB Path:   " << dbPath;

    // 2. Create the folder path if it's missing
    if (!dir.exists()) {
        if (dir.mkpath(".")) {
            qDebug() << "Successfully created directory:" << dir.absolutePath();
        } else {
            qDebug() << "CRITICAL ERROR: Could not create directory:" << dir.absolutePath();
        }
    }

    // 3. Copy from resources if the file doesn't exist yet
    if (!QFile::exists(fullPath)) {
        if (QFile::copy(":/res/project1.db", fullPath)) {
            // Very Important: Files copied from resources are read-only by default.
            // We must change permissions so SQLite can write to it.
            QFile::setPermissions(fullPath, QFileDevice::WriteOwner | QFileDevice::ReadOwner);
            qDebug() << "Database successfully copied to AppData.";
        } else {
            qDebug() << "Failed to copy database from resources!";
        }
    }

    // 4. Now open the database
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(fullPath);

    if (!m_db.open()) {
        qDebug() << "Database Error:" << m_db.lastError().text();
    } else {
        qDebug() << "Database is open and ready!";
    }
}

DbManager::~DbManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DbManager::isOpen() const {
    return m_db.isOpen();
}