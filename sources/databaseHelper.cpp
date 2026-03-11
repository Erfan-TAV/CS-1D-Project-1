#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QtSql>
#include "xlsxdocument.h"


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


// --- Helper for Route Calculation ---
// Calculates the direct distance between two specific campuses
double getDistanceBetween(int id1, int id2) {
    QSqlQuery query;
    query.prepare("SELECT distance FROM campusDistances WHERE "
                  "(campusID1 = :id1 AND campusID2 = :id2) OR "
                  "(campusID1 = :id2 AND campusID2 = :id1)");
    query.bindValue(":id1", id1);
    query.bindValue(":id2", id2);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 999999.0; // Return an artificially high number if no direct connection exists
}
// ------------------------------------

// ==========================================
// REFACTORED: ALGORITHM LOGIC (Requirement 1 & 4)
// ==========================================
void calculateEfficientTrip(int startCampusID, QList<int> unvisitedIDs) {
    qDebug() << "\n[ALGO] --- Starting Efficient Trip Calculation ---";
    qDebug() << "[ALGO] Origin ID:" << startCampusID << " | Targets count:" << unvisitedIDs.size();

    double totalDistance = 0.0;
    int currentID = startCampusID;
    int order = 1; // Start campus was order 0

    while (!unvisitedIDs.isEmpty()) {
        int nextID = -1;
        double minFound = std::numeric_limits<double>::max();

        // Find closest school from current location
        for (int targetID : unvisitedIDs) {
            double dist = getDistanceBetween(currentID, targetID); // Helper function
            if (dist < minFound) {
                minFound = dist;
                nextID = targetID;
            }
        }

        if (nextID != -1) {
            totalDistance += minFound;
            currentID = nextID;
            QString name = getCampusName(currentID); // Helper function

            qDebug() << "[ALGO] Step" << order << ": Found nearest ->" << name
                     << "(ID:" << currentID << ") at" << minFound << "miles.";

            // Log to trip table (Requirement 1: Logic outside of UI)
            addTripCampus(currentID, name, order++);

            unvisitedIDs.removeAll(currentID);
        } else {
            qDebug() << "[ALGO] ERROR: Could not find connection for ID" << currentID;
            break;
        }
    }

    qDebug() << "[ALGO] --- Calculation Complete ---";
    qDebug() << "[ALGO] Final Total Distance:" << totalDistance << "miles.\n";
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

int closestCampus(const int campusID) {
  QSqlQuery query;
  int minDistance = INT_MAX;
  int nearestID = -1;

         // The SQL logic:
         // 1. Check if our ID is in column 1 OR column 2.
         // 2. Use CASE to return the "other" ID as 'neighborID'.
  query.prepare("SELECT "
                "CASE WHEN campusID1 = :id THEN campusID2 ELSE campusID1 END AS neighborID, "
                "distance "
                "FROM campusDistances "
                "WHERE campusID1 = :id OR campusID2 = :id");

  query.bindValue(":id", campusID);

  if (!query.exec()) {
    qDebug() << "SQL ERROR:" << query.lastError().text();
    return -1;
  }

  while (query.next()) {
    int neighborID = query.value(0).toInt();
    int currentDist = query.value(1).toInt();

           // Standard greedy check for the minimum distance
    if (currentDist < minDistance) {
      minDistance = currentDist;
      nearestID = neighborID;
    }
  }

  if (nearestID == -1) {
    qDebug() << "No connections found for Campus ID:" << campusID;
  }

  return nearestID;
}

// --- File Upload Helpers ---

// TODO: notify if the campus already exists
QStringList uploadFileAppend(const QString &filePath) {
    QXlsx::Document xlsx(filePath);
    if (!xlsx.load()) {
        qDebug() << "[ERROR] Could not load Excel file at:" << filePath;
        return {};
    }

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QStringList newlyAdded;

    for (const QString &sheetName : xlsx.sheetNames()) {
        xlsx.selectSheet(sheetName);

        // Map headers to column numbers
        QMap<QString, int> headerMap;
        for (int col = 1; col <= 20; ++col) {
            QVariant header = xlsx.read(1, col);
            if (!header.isValid()) break;
            headerMap[header.toString().toLower().trimmed()] = col;
        }

        int row = 2;
        // ONLY process the specific sheet for its specific table
        if (sheetName.toLower() == "campuslist") {
            while (xlsx.read(row, 1).isValid()) {
                QString name = xlsx.read(row, headerMap["campusname"]).toString();
                if (!name.isEmpty() && addCampus(name)) {
                    newlyAdded << name;
                }
                row++;
            }
        }
        else if (sheetName.toLower() == "souvenirs") {
            while (xlsx.read(row, 1).isValid()) {
                int cID = xlsx.read(row, headerMap["campusid"]).toInt();
                QString sName = xlsx.read(row, headerMap["itemname"]).toString();
                double price = xlsx.read(row, headerMap["price"]).toDouble();
                addSouvenir(cID, sName, price);
                row++;
            }
        }
        else if (sheetName.toLower() == "campusdistances") {
            while (xlsx.read(row, 1).isValid()) {
                int id1 = xlsx.read(row, headerMap["campusid1"]).toInt();
                int id2 = xlsx.read(row, headerMap["campusid2"]).toInt();
                int dist = xlsx.read(row, headerMap["distance"]).toInt();
                // Add bi-directionally
                addDistance(id1, id2, dist);
                addDistance(id2, id1, dist);
                row++;
            }
        }
    }

    if (db.commit()) {
        return newlyAdded;
    } else {
        db.rollback();
        return {};
    }
}

void resetAndReloadData(const QString &filePath) {
    QXlsx::Document xlsx(filePath);
    if (!xlsx.load()) {
        qDebug() << "[ERROR] Could not load Excel file for reset:" << filePath;
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qDebug() << "Transaction failed:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    // 1. Wipe the specific tables in order to respect potential constraints
    QStringList tables = {"campusDistances", "souvenirs", "campusList"};
    for (const QString &table : tables) {
        if (!query.exec(QString("DELETE FROM %1").arg(table))) {
            qDebug() << "Failed to wipe table" << table << ":" << query.lastError().text();
            db.rollback();
            return;
        }
    }

    // 2. Iterate through Excel sheets and perform the direct insert
    for (const QString &sheetName : xlsx.sheetNames()) {
        xlsx.selectSheet(sheetName);

        // Get Headers
        QStringList columnNames;
        QStringList quotedColumns;
        QStringList placeholders;
        int col = 1;
        while (true) {
            QVariant headerCell = xlsx.read(1, col);
            if (!headerCell.isValid() || headerCell.isNull()) break;

            QString colName = headerCell.toString();
            columnNames << colName;
            quotedColumns << QString("\"%1\"").arg(colName);
            placeholders << "?";
            col++;
        }

        if (columnNames.isEmpty()) continue;

        // Prepare the INSERT query
        QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                          .arg(sheetName,
                               quotedColumns.join(", "),
                               placeholders.join(", "));

        query.prepare(sql);

        // 3. Process Rows (Starting from Row 2)
        int row = 2;
        while (true) {
            QVariant firstCell = xlsx.read(row, 1);
            if (!firstCell.isValid() || firstCell.isNull()) break;

            for (int c = 1; c <= columnNames.size(); ++c) {
                query.addBindValue(xlsx.read(row, c));
            }

            if (!query.exec()) {
                qDebug() << "[SQL ERROR] Reset Row" << row << "in" << sheetName << ":" << query.lastError().text();
            }
            row++;
        }
    }

    if (db.commit()) {
        qDebug() << "Database successfully reset and reloaded from Excel.";
    } else {
        qDebug() << "Commit failed during reset. Rolling back.";
        db.rollback();
    }
}

bool addTripCampus(const int campusID, const QString &campusName, const int visitOrder) {
  QSqlQuery query;

         // Updated to include visitOrder in the insert statement
  query.prepare("INSERT INTO tripCampuses (campusID, campusName, visitOrder) "
                "VALUES (?, ?, ?)");

  query.addBindValue(campusID);
  query.addBindValue(campusName);
  query.addBindValue(visitOrder);

  if (!query.exec()) {
    qDebug() << "DB Helper Error (Add):" << query.lastError().text();
    return false;
  }

  return true;
}

bool removeTripCampusByID(const int campusID) {
  QSqlQuery query;
  // Note: Since tripStopID is the primary key, usually you'd delete by that,
  // but deleting by campusID works if you want to remove all instances of that campus.
  query.prepare("DELETE FROM tripCampuses WHERE campusID = :id");
  query.bindValue(":id", campusID);

  if (!query.exec()) {
    qDebug() << "DB Helper Error (Remove by ID):" << query.lastError().text();
    return false;
  }
  return true;
}

bool removeTripCampusByName(const QString &campusName) {
  QSqlQuery query;
  query.prepare("DELETE FROM tripCampuses WHERE campusName = :name");
  query.bindValue(":name", campusName);

  if (!query.exec()) {
    qDebug() << "DB Helper Error (Remove by Name):" << query.lastError().text();
    return false;
  }
  return true;
}

bool clearTripTable() {
  QSqlQuery query;

  if (!query.exec("DELETE FROM tripCampuses")) {
    qDebug() << "DB Helper Error (Clear Table):" << query.lastError().text();
    return false;
  }

  // Optional: Reset the autoincrement counter so the next trip starts stop at ID 1
  query.exec("DELETE FROM sqlite_sequence WHERE name='tripCampuses'");

  return true;
}
