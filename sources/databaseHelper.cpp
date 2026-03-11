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
// 2. The Recursive Function
void calculateEfficientTrip(int currentID, QList<int> unvisitedIDs, double totalDistance, int order) {
    // BASE CASE: If there are no more campuses to visit, stop the recursion.
    if (unvisitedIDs.isEmpty()) {
        qDebug() << "[ALGO] --- Recursion Complete ---";
        qDebug() << "[ALGO] Final Total Distance:" << totalDistance << "miles.";
        return;
    }

    int nextID = -1;
    double minFound = std::numeric_limits<double>::max();

    // Find the single closest school from the current location
    for (int targetID : unvisitedIDs) {
        double dist = getDistanceBetween(currentID, targetID);
        if (dist < minFound) {
            minFound = dist;
            nextID = targetID;
        }
    }

    if (nextID != -1) {
        // Prepare data for the next stop
        double newTotalDistance = totalDistance + minFound;
        QString name = getCampusName(nextID);

        qDebug() << "[ALGO] Recursive Level" << order << ": Visiting" << name
                 << " (ID:" << nextID << ") +" << minFound << "mi.";

        // Record the visit in the database
        addTripCampus(nextID, name, order);

        // Remove the campus we just visited
        unvisitedIDs.removeAll(nextID);

        // RECURSIVE CALL: The function calls ITSELF with the new location and updated totals
        calculateEfficientTrip(nextID, unvisitedIDs, newTotalDistance, order + 1);
    } else {
        qDebug() << "[ALGO] ERROR: Path broken at ID" << currentID;
    }
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

    // 1. Fill in campus name and id into campus struct
    query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
    query.bindValue(":id", campusID);

    if (query.exec() && query.next()) {
        campus.campusID = campusID;
        campus.campusName = query.value(0).toString();
    } else {
        qDebug() << "Campus ID" << campusID << "not found.";
        return campus;
    }

    // 2. Fill in souvenir information into campus struct
    // FIXED: Changed itemName to souvenirName to match schema
    query.prepare("SELECT souvenirName, price FROM souvenirs WHERE campusID = :id");
    query.bindValue(":id", campusID);

    if (query.exec()) {
        while (query.next()) {
            campusSouvenir item;
            // FIXED: Using "souvenirName" for value retrieval
            item.name = query.value("souvenirName").toString();
            item.price = query.value("price").toDouble();
            campus.souvenirs.append(item);
        }
    }

    // 3. Fill in distance information
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
    // Maps Excel CampusID -> Actual Database CampusID
    QMap<int, int> IDMap;

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

        // 1. Process CampusList first to establish ID mapping
        if (sheetName.toLower() == "campuslist") {
            while (xlsx.read(row, 1).isValid()) {
                int excelID = xlsx.read(row, headerMap["campusid"]).toInt();
                QString name = xlsx.read(row, headerMap["campusname"]).toString();

                if (!name.isEmpty()) {
                    QSqlQuery query;
                    query.prepare("INSERT INTO campusList (campusName) VALUES (:name)");
                    query.bindValue(":name", name);

                    if (query.exec()) {
                        // Retrieve the ACTUAL ID generated by the database (Auto-increment)
                        int actualID = query.lastInsertId().toInt();
                        IDMap.insert(excelID, actualID);
                        newlyAdded << name;
                    } else {
                        qDebug() << "Failed to add campus" << name << ":" << query.lastError().text();
                    }
                }
                row++;
            }
        }
        // 2. Process Souvenirs using the map
        else if (sheetName.toLower() == "souvenirs") {
            while (xlsx.read(row, 1).isValid()) {
                int excelID = xlsx.read(row, headerMap["campusid"]).toInt();
                QString sName = xlsx.read(row, headerMap["souvenirname"]).toString();
                double price = xlsx.read(row, headerMap["price"]).toDouble();

                // Replace Excel ID with Actual ID from our map
                if (IDMap.contains(excelID)) {
                    int actualID = IDMap.value(excelID);
                    if (!addSouvenir(actualID, sName, price)) {
                        qDebug() << "Failed to add souvenir" << sName << "for actual ID" << actualID;
                    }
                }
                row++;
            }
        }
        // 3. Process Distances using the map
        else if (sheetName.toLower() == "campusdistances") {
            while (xlsx.read(row, 1).isValid()) {
                int excelID1 = xlsx.read(row, headerMap["campusid1"]).toInt();
                int excelID2 = xlsx.read(row, headerMap["campusid2"]).toInt();
                int dist = xlsx.read(row, headerMap["distance"]).toInt();

                // Only add if BOTH campuses were part of this new upload
                if (IDMap.contains(excelID1) && IDMap.contains(excelID2)) {
                    int actualID1 = IDMap.value(excelID1);
                    int actualID2 = IDMap.value(excelID2);
                    addDistance(actualID1, actualID2, dist);
                    addDistance(actualID2, actualID1, dist);
                }
                row++;
            }
        }
    }

    if (db.commit()) {
        qDebug() << "Successfully uploaded" << newlyAdded.size() << "campuses with re-mapped IDs.";
        return newlyAdded;
    } else {
        db.rollback();
        qDebug() << "Upload failed, transaction rolled back.";
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

bool populateTripSouvenirs()
{
    QSqlQuery query;

    // Clear previous trip purchases first
    if (!query.exec("DELETE FROM tripSouvenirPurchases")) {
        qDebug() << "Failed clearing tripSouvenirPurchases:" << query.lastError().text();
        return false;
    }

    // Copy souvenirs from campuses included in the trip
    QString sql =
        "INSERT INTO tripSouvenirPurchases (campusID, souvenirName, price) "
        "SELECT s.campusID, s.souvenirName, s.price "
        "FROM souvenirs s "
        "INNER JOIN tripCampuses t "
        "ON s.campusID = t.campusID";

    if (!query.exec(sql)) {
        qDebug() << "Failed populating tripSouvenirPurchases:" << query.lastError().text();
        return false;
    }

    qDebug() << "[DB] Trip souvenirs populated.";
    return true;
}

bool createTripInfoTable()
{
    QSqlQuery query;

    QString sql =
        "CREATE TABLE IF NOT EXISTS tripInfo ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "campusName TEXT,"
        "itemName TEXT,"
        "numItem INTEGER,"
        "itemPrice REAL,"
        "totalPrice REAL"
        ");";

    if(!query.exec(sql))
    {
        qDebug() << "Error creating tripInfo table:" << query.lastError().text();
        return false;
    }

    return true;
}
bool addTripInfo(const QString& campusName,
                 const QString& itemName,
                 int numItem,
                 double itemPrice)
{
    QSqlQuery query;
    double totalPrice = numItem * itemPrice;

    query.prepare("INSERT INTO tripInfo "
                  "(campusName, itemName, numItem, itemPrice, totalPrice) "
                  "VALUES (:campusName, :itemName, :numItem, :itemPrice, :totalPrice)");

    query.bindValue(":campusName", campusName);
    query.bindValue(":itemName", itemName);
    query.bindValue(":numItem", numItem);
    query.bindValue(":itemPrice", itemPrice);
    query.bindValue(":totalPrice", totalPrice);

    if (!query.exec()) {
        qDebug() << "addTripInfo error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool clearTripInfoTable()
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM tripInfo")) {
        qDebug() << "clearTripInfoTable error:" << query.lastError().text();
        return false;
    }
    return true;
}

double getTripInfoTotalSpent()
{
    QSqlQuery query;
    if (!query.exec("SELECT IFNULL(SUM(totalPrice), 0) FROM tripInfo")) {
        qDebug() << "getTripInfoTotalSpent error:" << query.lastError().text();
        return 0.0;
    }

    if (query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

int getTripInfoTotalItems()
{
    QSqlQuery query;
    if (!query.exec("SELECT IFNULL(SUM(numItem), 0) FROM tripInfo")) {
        qDebug() << "getTripInfoTotalItems error:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}
