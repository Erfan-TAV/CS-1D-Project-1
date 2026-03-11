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
 * @brief Fetches the distance between two campuses from the database.
 * @param id1 First campus ID
 * @param id2 Second campus ID
 * @return The distance in miles, or 999999.0 if no connection exists.
 */
double getDistanceBetween(const int id1, const int id2);

/**
 * @brief Recursively calculates the most efficient trip using the Nearest Neighbor algorithm.
 * @param currentID The ID of the campus currently being visited.
 * @param unvisitedIDs A list of IDs for campuses that still need to be visited.
 * @param totalDistance The cumulative distance traveled so far.
 * @param order The current sequence number of the stop (e.g., 1st, 2nd, 3rd).
 * @par Complexity: 
 * O(N^2) where N is the number of campuses. The function performs a linear search 
 * at each of the N recursive levels.
 */
void calculateEfficientTrip(int currentID, QList<int> unvisitedIDs, double totalDistance, int order);

/**
 * @brief Finds the nearest campus to a given location using a greedy search.
 * @param campusID The ID of the campus to start the search from.
 * @return The ID of the closest neighboring campus, or -1 if no connections exist.
 * @note Complexity: O(N) where N is the number of neighbors for the given campus.
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
 * @brief Parses an Excel file and appends new campus, souvenir, and distance data to the database.
 * @param filePath The absolute or relative path to the .xlsx file.
 * @return A QStringList containing the names of the campuses successfully added.
 * @par Complexity:
 * O(S * R) where S is the number of sheets and R is the number of rows per sheet. 
 * @warning This function loads the entire file into memory; large files may impact performance.
 */
QStringList uploadFileAppend(const QString &filePath);

/**
 * @brief Wipes the current database and reloads it entirely from an Excel file.
 * @param filePath The path to the master Excel data file.
 * @note This is used for "Reset to Default" functionality.
 */
void resetAndReloadData(const QString &filePath);

/**
 * @brief Records a campus visit in the temporary trip itinerary table.
 * @param campusID ID of the campus visited.
 * @param campusName Name of the campus.
 * @param visitOrder The sequence number in the trip (1st, 2nd, etc.).
 * @return true if added to the itinerary, false otherwise.
 */
bool addTripCampus(const int campusID, const QString& campusName, const int visitOrder);

/**
 * @brief Removes a single campus from the itinerary by ID.
 * @param campusID The ID of the stop to remove.
 * @return true if removed successfully.
 */
bool removeTripCampus(const int campusID);

/**
 * @brief Removes a single campus from the itinerary by Name.
 * @param campusName The name of the stop to remove.
 * @return true if removed successfully.
 */
bool removeTripCampusByName(const QString &campusName);

/**
 * @brief Clears all stops from the current trip itinerary.
 * @return true if table was successfully wiped.
 */
bool clearTripTable();

// --- Trip Information & Analytics ---

/**
 * @brief Populates the purchaseable souvenirs for all campuses currently in the trip itinerary.
 * @return true if information was synced successfully.
 */
bool populateTripSouvenirs();

/**
 * @brief Initializes the table used to track souvenir purchases during a trip.
 * @return true if table is ready for use.
 */
bool createTripInfoTable();

/**
 * @brief Records a souvenir purchase at a specific campus during the trip.
 * @param campusName Name of the campus where item was bought.
 * @param itemName Name of the souvenir.
 * @param numItem Quantity purchased.
 * @param itemPrice Price at the time of purchase.
 * @param campusID ID of the campus.
 * @return true if purchase was recorded.
 */
bool addTripInfo(const QString& campusName,
                 const QString& itemName,
                 int numItem,
                 double itemPrice,
                 const int campusID);

/**
 * @brief Wipes all recorded purchases for the current trip.
 */
bool clearTripInfoTable();

/**
 * @brief Calculates the total cost of all souvenirs purchased during the trip.
 * @return Total dollar amount spent.
 */
double getTripInfoTotalSpent();

/**
 * @brief Calculates the total number of items purchased across all campuses.
 * @return Total count of souvenirs bought.
 */
int getTripInfoTotalItems();

/** @} */ // End of DB_Helpers group

#endif //COLLEGETOUR_DBMANAGERHELPER_H
