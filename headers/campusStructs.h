//
// Created by erfan on 2/25/2026.
//

/**
 * @file CampusStructs.h
 * @brief Definition of data structures used for campus management and navigation.
 * * This file contains the primary structures used to represent campuses, 
 * including their souvenirs and spatial relationships to other campuses.
 */
#ifndef COLLEGETOUR_CAMPUSSTRUCTS_H
#define COLLEGETOUR_CAMPUSSTRUCTS_H
#include <qlist.h>
#include <QString>

/**
 * @struct campusSouvenir
 * @brief Represents a souvenir item available at a specific campus.
 */
struct campusSouvenir {
    QString name; /**< The name of the souvenir (e.g., "Sweatshirt", "Keychain"). */
    double price; /**< The cost of the souvenir. */
};

/**
 * @struct CampusDistances
 * @brief Represents the distance from one campus to another.
 * 
 */
struct CampusDistances {
    int otherCampusID; /**< The unique ID of the destination campus. */
    double distance; /**< The distance to the other campus in miles. */
};
/**
 * @struct Campus
 * @brief Core structure containing all relevant data for a single University campus.
 */
struct Campus {
    int campusID; /**< Unique identifier for the campus. */
    QString campusName; /**< Name of the university. */
    QList<CampusDistances> distances; /**< List of distances to all other connected campuses. */
    QList<campusSouvenir> souvenirs; /**< List of souvenirs available for purchase at this campus. */
};



#endif //COLLEGETOUR_CAMPUSSTRUCTS_H