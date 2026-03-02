#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

#include "databaseHelper.h"
#include "campusStructs.h"

// --- Campus Helpers ---

bool addCampus(const QString& name) {
    QSqlQuery query;
    query.prepare("INSERT INTO campusList (campusName) VALUES (:name)");
    query.bindValue(":name", name);

    if(!query.exec()) {
        qDebug() << "addCampus error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool removeCampus(const int campusID) {
    QSqlQuery query;
    // Note: You should ideally enable Foreign Key constraints so deleting
    // a campus also removes its souvenirs/distances.
    query.prepare("DELETE FROM campusList WHERE campusID = :id");
    query.bindValue(":id", campusID);
    return query.exec();
}

// --- Souvenir Helpers ---

bool addSouvenir(const int campusID, const QString& name, const double price) {
    QSqlQuery query;
    query.prepare("INSERT INTO souvenirs (campusID, souvenirName, price) "
                  "VALUES (:id, :name, :price)");
    query.bindValue(":id", campusID);
    query.bindValue(":name", name);
    query.bindValue(":price", price);
    return query.exec();
}

bool updateSouvenirPrice(const int campusID, const QString& name, const double newPrice) {
    QSqlQuery query;
    query.prepare("UPDATE souvenirs SET price = :price "
                  "WHERE campusID = :id AND souvenirName = :name");
    query.bindValue(":price", newPrice);
    query.bindValue(":id", campusID);
    query.bindValue(":name", name);
    return query.exec();
}

bool removeSouvenir(const int campusID, const QString& name) {
    QSqlQuery query;

    query.prepare("DELETE FROM souvenirs WHERE campusID = :id AND souvenirName = :name");
    query.bindValue(":id", campusID);
    query.bindValue(":name", name);

    return query.exec();
}

// --- Distance Helpers ---

bool addDistance(const int id1, const int id2, const int distance) {
    QSqlQuery query;

    query.prepare("INSERT INTO campusDistances (campusID1, campusID2, distance) "
                  "VALUES (:id1, :id2, :dist)");
    query.bindValue(":id1", id1);
    query.bindValue(":id2", id2);
    query.bindValue(":dist", distance);

    return query.exec();
}

QString getCampusName(const int campusID) {
    QSqlQuery query;

    // SQL Query
    query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
    query.bindValue(":id", campusID);

    // check if sql query ran and output correlating messages
    if (query.exec()) {
        if (query.next()) {
            return query.value(0).toString();
        } else {
            qDebug() << "No campus found with ID:" << campusID;
        }
    } else {
        qDebug() << "Database Query Error:" << query.lastError().text();
    }

    return {};
}

Campus getFullCampus(const int campusID) {
    Campus campus;
    campus.campusID = -1; // default case if not found

    QSqlQuery query;

    // 1. fill in campus name and id into campus struct
    query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
    query.bindValue(":id", campusID);

    if (query.exec() && query.next()) {
        campus.campusID = campusID;
        campus.campusName = query.value(0).toString();
    } else {
        qDebug() << "Campus ID" << campusID << "not found.";
        return campus;
    }

    // 2. fill in souvenir information into campus struct
    query.prepare("SELECT itemName, price FROM souvenirs WHERE campusID = :id");
    query.bindValue(":id", campusID);

    if (query.exec()) {
        while (query.next()) {
            campusSouvenir item;
            item.name = query.value("itemName").toString();
            item.price = query.value("price").toDouble();
            campus.souvenirs.append(item);
        }
    }

    // 3. fill in distance information
    query.prepare("SELECT otherCampusID, distance FROM campusDistances WHERE campusID = :id");
    query.bindValue(":id", campusID);

    if (query.exec()) {
        while (query.next()) {
            CampusDistances dist;
            dist.otherCampusID = query.value("otherCampusID").toInt();
            dist.distance = query.value("distance").toDouble();
            campus.distances.append(dist);
        }
    }

    return campus;
}

int closestCampus(const int ID1) {
    QSqlQuery query;
    int minDistance = INT_MAX;
    int nearestID = -1;

    // 1. Prepare the string first
    query.prepare("SELECT campusID2, distance FROM campusDistances WHERE campusID1 = :id");

    // 2. Bind the actual value to the placeholder
    query.bindValue(":id", ID1);

    // 3. Call exec() with NO arguments
    if (!query.exec()) {
        qDebug() << "SQL ERROR:" << query.lastError().text();
        return -1;
    }

    int rowCount = 0;
    while (query.next()) {
        rowCount++;
        int currentID2 = query.value(0).toInt();
        int currentDist = query.value(1).toInt();

        if (currentDist < minDistance) {
            minDistance = currentDist;
            nearestID = currentID2;
        }
    }

    if (rowCount == 0) {
        qDebug() << "Zero rows found for ID1 =" << ID1;
    }

    return nearestID;
}
