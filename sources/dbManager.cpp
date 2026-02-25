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

// --- Campus Helpers ---

bool DbManager::addCampus(const QString& name) {
    QSqlQuery query;
    query.prepare("INSERT INTO campusList (campusName) VALUES (:name)");
    query.bindValue(":name", name);

    if(!query.exec()) {
        qDebug() << "addCampus error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::removeCampus(int campusID) {
    QSqlQuery query;
    // Note: You should ideally enable Foreign Key constraints so deleting
    // a campus also removes its souvenirs/distances.
    query.prepare("DELETE FROM campusList WHERE campusID = :id");
    query.bindValue(":id", campusID);
    return query.exec();
}

// --- Souvenir Helpers ---

bool DbManager::addSouvenir(int campusID, const QString& name, double price) {
    QSqlQuery query;
    query.prepare("INSERT INTO souvenirs (campusID, souvenirName, price) "
                  "VALUES (:id, :name, :price)");
    query.bindValue(":id", campusID);
    query.bindValue(":name", name);
    query.bindValue(":price", price);
    return query.exec();
}

bool DbManager::updateSouvenirPrice(int campusID, const QString& name, double newPrice) {
    QSqlQuery query;
    query.prepare("UPDATE souvenirs SET price = :price "
                  "WHERE campusID = :id AND souvenirName = :name");
    query.bindValue(":price", newPrice);
    query.bindValue(":id", campusID);
    query.bindValue(":name", name);
    return query.exec();
}

bool DbManager::removeSouvenir(int campusID, const QString& name) {
    QSqlQuery query;
    query.prepare("DELETE FROM souvenirs WHERE campusID = :id AND souvenirName = :name");
    query.bindValue(":id", campusID);
    query.bindValue(":name", name);
    return query.exec();
}

// --- Distance Helpers ---

bool DbManager::addDistance(int id1, int id2, int distance) {
    QSqlQuery query;
    query.prepare("INSERT INTO campusDistances (campusID1, campusID2, distance) "
                  "VALUES (:id1, :id2, :dist)");
    query.bindValue(":id1", id1);
    query.bindValue(":id2", id2);
    query.bindValue(":dist", distance);
    return query.exec();
}