/**
 * @file databaseHelper.h
 * @brief Utilities for managing the SQLite campus database.
 * @author Erfan Tavassoli
 */
#ifndef COLLEGETOUR_DBMANAGERHELPER_H
#define COLLEGETOUR_DBMANAGERHELPER_H

#include "campusStructs.h"

/**
 * @defgroup DB_Helpers Database Helper Functions
 * @brief  Standalone functions to interface with the SQLite tables/database.
 * @{
 */

// --- Campus Helpers ---

/**
 * @brief Adds a new campus to the database
 * @param name The name of the college/campus being added
 * @return true if inserted successfully, false otherwise
 */
bool addCampus(const QString& name);
/**
 * @brief Deletes a campus from the database based on ID
 * @param campusID The unique identifier of the campus
 * @return true if deleted successfully, false otherwise
 */
bool removeCampus(int campusID);

// --- Souvenir Helpers ---

/**
 * @brief Adds a souvenir to a specific campus's inventory
 * @param campusID ID of the campus providing the souvenir
 * @param name souvenir items name
 * @param price Cost of the souvenir
 * @return true if added successfully, false otherwise
 */
bool addSouvenir(int campusID, const QString& name, double price);
/**
 * @brief Updates the price of an existing souvenir
 * @param campusID ID of the campus
 * @param name Name of the souvenir to update
 * @param newPrice The updated cost
 * @return true if the price was updated, false otherwise
 */
bool updateSouvenirPrice(int campusID, const QString& name, double newPrice);
/**
 * @brief Removes a souvenir from a campus's inventory
 * @param campusID ID of the campus
 * @param name Name of the souvenir to remove
 * @return true if deleted successfully, false otherwise
 */
bool removeSouvenir(int campusID, const QString& name);

// --- Distance Helpers ---

/**
 * @brief Adds a distance record between two campuses
 * @param id1 Starting campus ID
 * @param id2 Destination campus ID
 * @param distance The distance between them
 * @return true if distance was recorded, false otherwise
 */
bool addDistance(int id1, int id2, int distance);

double getDistanceBetween(const int id1, const int id2);
void calculateEfficientTrip(int currentID, QList<int> unvisitedIDs, double totalDistance, int order);

/**
 * @brief Finds the closest campus to a specific ID
 * @param campusID The ID of the current campus
 * @return The ID of the nearest campus, or -1 if none found
 */
int closestCampus(int campusID);

/**
 * @brief Retrieves the human-readable name of a campus
 * @param campusID The ID of the campus
 * @return QString containing the name associated with the campusID, or empty if not found
 */
QString getCampusName(int campusID);
/**
 * @brief Fetches all data (including souvenirs and distances) for a single campus
 * @param campusID The ID of the campus to fetch information
 * @return A Campus struct populated with campusID, name and qlist of souvenirs and distances
 */
Campus getFullCampus(int campusID);

// File Upload
// TODO: add doxy
QStringList uploadFileAppend(const QString &filePath);
// TODO: add doxy
void resetAndReloadData(const QString &filePath);

// TODO: add doxy
bool addTripCampus(const int campusID, const QString &campusName, const int visitOrder);
// TODO: add doxy
bool removeTripCampus(const int campusID);
// TODO: add doxy
bool removeTripCampusByName(const QString &campusName);
// TODO: add doxy
bool clearTripTable();

bool populateTripSouvenirs();

#endif //COLLEGETOUR_DBMANAGERHELPER_H
