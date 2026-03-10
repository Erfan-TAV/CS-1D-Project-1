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
/**
 * @brief Parses a file and appends new campus or souvenir data to the existing database.
 * @param filePath The path to the source file.
 * @return A QStringList containing any error messages or a log of imported items.
 */
QStringList uploadFileAppend(const QString &filePath);
/**
 * @brief Wipes the current database and re-initializes it using data from a specific file.
 * @warning This will result in the loss of all current session data.
 * @param filePath The path to the initialization file.
 */
void resetAndReloadData(const QString &filePath);

/**
 * @brief Adds a specific campus to the current trip itinerary.
 * @param campusID The unique ID of the campus to add.
 * @param campusName The name of the campus to add.
 * @return true if the campus was successfully added, false if it already exists in the trip.
 */
bool addTripCampus(const int campusID, const QString& campusName);
/**
 * @brief Removes a campus from the current trip itinerary using its ID.
 * @param campusID The unique ID of the campus to remove.
 * @return true if the removal was successful, false if the ID was not found in the trip.
 */
bool removeTripCampus(const int campusID);
/**
 * @brief Removes a campus from the current trip itinerary using its name.
 * @param campusName The string name of the campus to remove.
 * @return true if the removal was successful, false if the name was not found.
 */
bool removeTripCampusByName(const QString &campusName);
/**
 * @brief Clears all campuses from the current trip itinerary table.
 * @return true if the table was successfully cleared or was already empty.
 */
bool clearTripTable();

#endif //COLLEGETOUR_DBMANAGERHELPER_H
