//SQLite Database Manager Class
//need to add actual adding/changing/deletioin
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>

#include "campusStructs.h"

class DbManager {
public:
    // Constructor: takes bath to .db file
    DbManager(const QString& path);

    // Destructor: closes the file connection
    ~DbManager();

    // Check if the database is actually open
    bool isOpen() const;

    // --- Helper Functions
    bool addCampus(const QString& campus);
    bool removeCampus(int campusID);
    QString getCampusName(int campusID);
    Campus getFullCampus(int campusID);

    bool addSouvenir(int campusID, const QString& name, double price);
    bool updateSouvenirPrice(int campusID, const QString& name, double newPrice);
    bool removeSouvenir(int campusID, const QString& name);

    bool addDistance(int id1, int id2, int distance);

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H