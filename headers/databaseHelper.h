/**
 * @file databaseHelper.h
 * @brief Global utility functions for database CRUD operations and trip algorithms.
 * @author Erfan Tavassoli
 */

#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QString>
#include <QStringList>
#include <QList>
#include "campusStructs.h"

// --- Campus CRUD Helpers ---

/**
 * @brief Adds a new campus to the system.
 * @param name The name of the university.
 * @return true if the insertion was successful.
 */
bool addCampus(const QString& name);

/**
 * @brief Removes a campus record from the database.
 * @param campusID The unique ID of the campus to delete.
 * @return true if the execution was successful.
 * @note Without foreign key constraints, this may leave orphaned souvenirs or distances.
 */
bool removeCampus(const int campusID);

/**
 * @brief Retrieves the formal name of a campus.
 * @param campusID The ID to look up.
 * @return The name string, or an empty string if not found.
 */
QString getCampusName(const int campusID);

/**
 * @brief Fetches a complete data aggregate for a specific campus.
 * @param campusID The ID of the campus.
 * @return A Campus struct populated with name, souvenirs, and distances.
 */
Campus getFullCampus(const int campusID);


// --- Souvenir Helpers ---

/**
 * @brief Registers a new souvenir item for a specific campus.
 * @param campusID The owner campus ID.
 * @param name Name of the souvenir.
 * @param price Retail price.
 * @return true if successful.
 */
bool addSouvenir(const int campusID, const QString& name, const double price);

/**
 * @brief Updates the pricing of an existing souvenir.
 * @param campusID The owner campus ID.
 * @param name The name of the item to update.
 * @param newPrice The updated price.
 * @return true if the update query executed successfully.
 */
bool updateSouvenirPrice(const int campusID, const QString& name, const double newPrice);

/**
 * @brief Deletes a souvenir from a campus gift shop.
 */
bool removeSouvenir(const int campusID, const QString& name);


// --- Distance & Graph Helpers ---

/**
 * @brief Adds a directed edge between two campuses.
 * @param id1 Source campus.
 * @param id2 Destination campus.
 * @param distance Weight of the edge in miles.
 */
bool addDistance(const int id1, const int id2, const int distance);

/**
 * @brief Fetches the distance between two nodes in the graph.
 * @details Checks both directions (A to B and B to A) to ensure pathing works.
 * @return The distance in miles, or 999999.0 if no connection exists.
 */
double getDistanceBetween(int id1, int id2);

/**
 * @brief Simple greedy search for the closest neighboring campus.
 * @param campusID The starting node.
 * @return The ID of the nearest neighbor, or -1 if isolated.
 */
int closestCampus(const int campusID);


// --- Algorithm Logic ---

/**
 * @brief Recursively calculates the most efficient route using a Nearest Neighbor approach.
 * @details This function implements a greedy algorithm to visit all specified campuses. 
 * It records each step of the trip into the 'tripCampuses' database table for UI display.
 * @param currentID The ID of the campus currently being "visited".
 * @param unvisitedIDs The list of campuses remaining in the itinerary.
 * @param totalDistance The cumulative distance traveled so far.
 * @param order The sequence number of the current stop.
 */
void calculateEfficientTrip(int currentID, QList<int> unvisitedIDs, double totalDistance, int order);




// --- File Upload & Maintenance ---

/**
 * @brief Parses an Excel file to append new data to the current database.
 * @details Performs a complex mapping of Excel IDs to database auto-increment IDs 
 * to ensure relational integrity across campuses, souvenirs, and distances.
 * @param filePath Path to the .xlsx file.
 * @return A list of names of the campuses successfully imported.
 */
QStringList uploadFileAppend(const QString &filePath);

/**
 * @brief Wipes the current database and reloads it entirely from an Excel source.
 * @details Uses a transaction to safely clear campusList, souvenirs, and campusDistances
 * before performing batch inserts from the Excel sheets.
 * @param filePath Path to the master .xlsx file.
 */
void resetAndReloadData(const QString &filePath);


// --- Trip Management (Itinerary & Purchases) ---

/**
 * @brief Logs a campus visit into the temporary trip itinerary table.
 */
bool addTripCampus(const int campusID, const QString &campusName, const int visitOrder);

/**
 * @brief Wipes the temporary itinerary table.
 */
bool clearTripTable();

/**
 * @brief Populates the purchase list with all available souvenirs from the planned trip.
 */
bool populateTripSouvenirs();

/**
 * @brief Records a souvenir purchase during a trip.
 * @details If the item already exists for that campus in the trip record, 
 * it increments the quantity and updates the total price instead of creating a duplicate.
 */
bool addTripInfo(const QString& campusName, const QString& itemName, int numItem, double itemPrice, const int campusID);

/**
 * @brief Returns the total dollar amount spent across the entire trip.
 */
double getTripInfoTotalSpent();

/**
 * @brief Returns the total count of items purchased during the trip.
 */
int getTripInfoTotalItems();

/**
 * @brief Clears all purchase records for the current trip.
 */
bool clearTripInfoTable();

#endif // DATABASEHELPER_H
