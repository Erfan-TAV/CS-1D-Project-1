//Database Management stuff (needs add/edit/delete stuff still)

#include "dbManager.h"

DbManager::DbManager(const QString& path) {
    //initializes he SQlite driver 
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    // opens the database 
    if (!m_db.open()) {
        qDebug() << "Error: connection with database failed" << m_db.lastError().text();
    } else {
        qDebug() << "Database: connection ok";

        // creates new database tables if they aren't already there
        QSqlQuery query;

        // Create campusList Table
        bool success = query.exec("CREATE TABLE IF NOT EXISTS campusList ("
                                  "campusID INTEGER NOT NULL UNIQUE, "
                                  "campusName TEXT, "
                                  "PRIMARY KEY(campusID))");
        if(!success) qDebug() << "Error creating campusList:" << query.lastError().text();

        // Create souvenirs Table
        success = query.exec("CREATE TABLE IF NOT EXISTS souvenirs ("
                             "campusID INTEGER, "
                             "souvenirName TEXT, "
                             "price REAL, "
                             "FOREIGN KEY(campusID) REFERENCES campusList(campusID))");
        if(!success) qDebug() << "Error creating souvenirs:" << query.lastError().text();

        // Create campusDistances Table
        success = query.exec("CREATE TABLE IF NOT EXISTS campusDistances ("
                             "campusID1 INTEGER, "
                             "campusID2 INTEGER, "
                             "distance INTEGER)");
        if(!success) qDebug() << "Error creating campusDistances:" << query.lastError().text();
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