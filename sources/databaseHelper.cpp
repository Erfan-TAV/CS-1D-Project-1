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

// TODO: update logic to work with no repeated campusID in the first colum. needs the logic to find either id1 or id2 and then the other id is the other campus
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

// --- File Upload Helpers ---

// TODO: notify if the campus already exists
// TODO: bring up a popup for adding distances to existing campuses
void uploadFileAppend(const QString &filePath) {
    QXlsx::Document xlsx(filePath);
    if (!xlsx.load()) {
        qDebug() << "[ERROR] Could not load Excel file at:" << filePath;
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    // 1. Find the next available campusID
    int nextID = 1;
    QSqlQuery idQuery("SELECT MAX(campusID) FROM campusList");
    if (idQuery.next() && !idQuery.value(0).isNull()) {
        nextID = idQuery.value(0).toInt() + 1;
    }

    // Map to keep track of multiple campuses within the same file if needed
    // Key: Original ID from Excel, Value: New ID generated for Database
    QMap<int, int> idMap;
    int currentIDToUse = nextID;

    for (const QString &sheetName : xlsx.sheetNames()) {
        xlsx.selectSheet(sheetName);

        // 2. Map Column Names
        QStringList columnNames;
        QStringList quotedColumns;
        int idColIndex = -1; // Track which column is the ID column
        int col = 1;

        while (true) {
            QVariant headerCell = xlsx.read(1, col);
            if (!headerCell.isValid() || headerCell.isNull()) break;

            QString colName = headerCell.toString();
            // Check if this column is an ID field we want to override
            if (colName.toLower() == "campusid" || colName.toLower() == "campusid1") {
                idColIndex = col;
            }

            columnNames << colName;
            quotedColumns << QString("\"%1\"").arg(colName);
            col++;
        }

        if (columnNames.isEmpty()) continue;

        // 3. Prepare Query
        QStringList placeholders;
        for (int i = 0; i < columnNames.size(); ++i) placeholders << "?";

        QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                          .arg(sheetName,
                               quotedColumns.join(", "),
                               placeholders.join(", "));

        QSqlQuery query;
        query.prepare(sql);

        // 4. Process Rows
        int row = 2;
        while (true) {
            QVariant firstCell = xlsx.read(row, 1);
            if (!firstCell.isValid() || firstCell.isNull()) break;

            // Handle ID mapping for multiple campuses in one file
            int excelID = xlsx.read(row, idColIndex).toInt();
            if (excelID > 0) {
                if (!idMap.contains(excelID)) {
                    idMap[excelID] = nextID++;
                }
                currentIDToUse = idMap[excelID];
            } else {
                currentIDToUse = nextID; // Fallback for single campus
            }

            for (int c = 1; c <= columnNames.size(); ++c) {
                QString currentHeader = columnNames.at(c-1).toLower();

                // INJECT our new ID instead of Excel's ID
                if (currentHeader == "campusid" || currentHeader == "campusid1") {
                    query.addBindValue(currentIDToUse);
                }
                // Special case for distances: update destination ID if it was mapped
                else if (currentHeader == "campusid2") {
                    int excelDestID = xlsx.read(row, c).toInt();
                    query.addBindValue(idMap.contains(excelDestID) ? idMap[excelDestID] : excelDestID);
                }
                else {
                    query.addBindValue(xlsx.read(row, c));
                }
            }

            if (!query.exec()) {
                qDebug() << "[SQL ERROR]" << sheetName << "Row" << row << ":" << query.lastError().text();
            }
            row++;
        }
    }

    db.commit();
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
