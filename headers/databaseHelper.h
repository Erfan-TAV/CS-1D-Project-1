#ifndef COLLEGETOUR_DBMANAGERHELPER_H
#define COLLEGETOUR_DBMANAGERHELPER_H

#include <QSqlQuery>
#include "campusStructs.h"

// --- Campus Helpers ---

bool addCampus(const QString& name);
bool removeCampus(int campusID);

// --- Souvenir Helpers ---

bool addSouvenir(int campusID, const QString& name, double price);
bool updateSouvenirPrice(int campusID, const QString& name, double newPrice);
bool removeSouvenir(int campusID, const QString& name);

// --- Distance Helpers ---
bool addDistance(int id1, int id2, int distance);
int closestCampus(int campusID);

QString getCampusName(int campusID);
Campus getFullCampus(int campusID);

// File Upload
void uploadFileAppend(const QString &filePath);
void uploadFileOverride(const QString &filePath);

#endif //COLLEGETOUR_DBMANAGERHELPER_H
