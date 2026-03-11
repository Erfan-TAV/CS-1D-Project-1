#include <QSqlQuery>
#include <QDebug>
#include <QtSql>
#include <QList>
#include <QMap>
#include <QStringList>
#include <limits>
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
  // Note: Foreign Key constraints should be enabled in the DB connection
  // to cascade delete souvenirs/distances.
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
  return 999999.0;
}

QString getCampusName(const int campusID) {
  QSqlQuery query;
  query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
  query.bindValue(":id", campusID);
  if (query.exec()) {
    if (query.next()) {
      return query.value(0).toString();
    }
  } else {
    qDebug() << "Database Query Error:" << query.lastError().text();
  }
  return {};
}

// --- Trip/Algorithm Helpers ---
bool addTripCampus(const int campusID, const QString &campusName, const int visitOrder) {
  QSqlQuery query;
  query.prepare("INSERT INTO tripCampuses (campusID, campusName, visitOrder) "
                "VALUES (?, ?, ?)");
  query.addBindValue(campusID);
  query.addBindValue(campusName);
  query.addBindValue(visitOrder);
  if (!query.exec()) {
    qDebug() << "DB Helper Error (Add Trip Campus):" << query.lastError().text();
    return false;
  }
  return true;
}

void calculateEfficientTrip(int currentID, QList<int> unvisitedIDs, double totalDistance, int order) {
  if (unvisitedIDs.isEmpty()) {
    qDebug() << "[ALGO] --- Recursion Complete ---";
    qDebug() << "[ALGO] Final Total Distance:" << totalDistance << "miles.";
    return;
  }

  int nextID = -1;
  double minFound = std::numeric_limits<double>::max();

  for (int targetID : unvisitedIDs) {
    double dist = getDistanceBetween(currentID, targetID);
    if (dist < minFound) {
      minFound = dist;
      nextID = targetID;
    }
  }

  if (nextID != -1) {
    double newTotalDistance = totalDistance + minFound;
    QString name = getCampusName(nextID);
    qDebug() << "[ALGO] Recursive Level" << order << ": Visiting" << name << " +" << minFound << "mi.";

    addTripCampus(nextID, name, order);
    unvisitedIDs.removeAll(nextID);
    calculateEfficientTrip(nextID, unvisitedIDs, newTotalDistance, order + 1);
  } else {
    qDebug() << "[ALGO] ERROR: Path broken at ID" << currentID;
  }
}

Campus getFullCampus(const int campusID) {
  Campus campus;
  campus.campusID = -1;
  QSqlQuery query;

  query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
  query.bindValue(":id", campusID);
  if (query.exec() && query.next()) {
    campus.campusID = campusID;
    campus.campusName = query.value(0).toString();
  } else {
    return campus;
  }

  query.prepare("SELECT souvenirName, price FROM souvenirs WHERE campusID = :id");
  query.bindValue(":id", campusID);
  if (query.exec()) {
    while (query.next()) {
      campusSouvenir item;
      item.name = query.value("souvenirName").toString();
      item.price = query.value("price").toDouble();
      campus.souvenirs.append(item);
    }
  }

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

// --- File Upload & Reset ---
QStringList uploadFileAppend(const QString &filePath) {
  QXlsx::Document xlsx(filePath);
  if (!xlsx.load()) return {};

  QSqlDatabase db = QSqlDatabase::database();
  db.transaction();
  QStringList newlyAdded;

  for (const QString &sheetName : xlsx.sheetNames()) {
    xlsx.selectSheet(sheetName);
    QString lowerName = sheetName.toLower();

    QMap<QString, int> headerMap;
    for (int col = 1; col <= 20; ++col) {
      QVariant header = xlsx.read(1, col);
      if (!header.isValid()) break;
      headerMap[header.toString().toLower().trimmed()] = col;
    }

    int row = 2;
    if (lowerName == "campuslist") {
      while (xlsx.read(row, 1).isValid()) {
        int campusID = xlsx.read(row, headerMap["campusid"]).toInt();
        QString campusName = xlsx.read(row, headerMap["campusname"]).toString();
        if (!campusName.isEmpty()) {
          QSqlQuery query;
          query.prepare("INSERT INTO campusList (campusID, campusName) VALUES (:id, :name)");
          query.bindValue(":id", campusID);
          query.bindValue(":name", campusName);
          if (query.exec()) newlyAdded << campusName;
        }
        row++;
      }
    }
    else if (lowerName == "souvenirs") {
      while (xlsx.read(row, 1).isValid()) {
        addSouvenir(xlsx.read(row, headerMap["campusid"]).toInt(),
                    xlsx.read(row, headerMap["souvenirname"]).toString(),
                    xlsx.read(row, headerMap["price"]).toDouble());
        row++;
      }
    }
  }
  if (db.commit()) return newlyAdded;
  db.rollback();
  return {};
}

void resetAndReloadData(const QString &filePath) {
  QXlsx::Document xlsx(filePath);
  if (!xlsx.load()) return;

  QSqlDatabase db = QSqlDatabase::database();
  if (!db.transaction()) return;

  QSqlQuery query;
  QStringList tables = {"campusDistances", "souvenirs", "campusList"};
  for (const QString &table : tables) {
    query.exec(QString("DELETE FROM %1").arg(table));
  }

  for (const QString &sheetName : xlsx.sheetNames()) {
    xlsx.selectSheet(sheetName);
    QStringList columnNames, placeholders, quotedColumns;
    int col = 1;
    while (true) {
      QVariant header = xlsx.read(1, col++);
      if (!header.isValid()) break;
      columnNames << header.toString();
      quotedColumns << QString("\"%1\"").arg(header.toString());
      placeholders << "?";
    }
    if (columnNames.isEmpty()) continue;

    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                      .arg(sheetName, quotedColumns.join(", "), placeholders.join(", "));

    int row = 2;
    while (xlsx.read(row, 1).isValid()) {
      query.prepare(sql);
      for (int c = 1; c <= columnNames.size(); ++c) {
        query.addBindValue(xlsx.read(row, c));
      }
      query.exec();
      row++;
    }
  }
  db.commit();
}

// --- Trip Maintenance ---
bool clearTripTable() {
  QSqlQuery query;
  query.exec("DELETE FROM tripCampuses");
  query.exec("DELETE FROM sqlite_sequence WHERE name='tripCampuses'");
  return true;
}

bool addTripInfo(const QString& campusName, const QString& itemName, int numItem, double itemPrice, const int campusID) {
  if (numItem <= 0 || itemPrice <= 0.0) return false;
  QSqlQuery query;
  query.prepare("SELECT numItem FROM tripInfo WHERE campusID = :campusID AND itemName = :itemName");
  query.bindValue(":campusID", campusID);
  query.bindValue(":itemName", itemName);

  if (query.exec() && query.next()) {
    int newNum = query.value(0).toInt() + numItem;
    QSqlQuery up;
    up.prepare("UPDATE tripInfo SET numItem = :n, totalPrice = :t WHERE campusID = :c AND itemName = :i");
    up.bindValue(":n", newNum);
    up.bindValue(":t", newNum * itemPrice);
    up.bindValue(":c", campusID);
    up.bindValue(":i", itemName);
    return up.exec();
  } else {
    QSqlQuery ins;
    ins.prepare("INSERT INTO tripInfo (campusName, itemName, numItem, itemPrice, totalPrice, campusID) "
                "VALUES (:cn, :in, :ni, :ip, :tp, :id)");
    ins.bindValue(":cn", campusName);
    ins.bindValue(":in", itemName);
    ins.bindValue(":ni", numItem);
    ins.bindValue(":ip", itemPrice);
    ins.bindValue(":tp", numItem * itemPrice);
    ins.bindValue(":id", campusID);
    return ins.exec();
  }
}

bool clearTripInfoTable() {
  QSqlQuery query;
  return query.exec("DELETE FROM tripInfo");
}

double getTripInfoTotalSpent() {
  QSqlQuery query;
  if (query.exec("SELECT SUM(totalPrice) FROM tripInfo") && query.next())
    return query.value(0).toDouble();
  return 0.0;
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
