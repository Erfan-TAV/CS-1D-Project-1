/**
* @file campusStructs.h
 * @brief Definition of a campus object
 * @author Erfan Tavassoli
 */

#ifndef COLLEGETOUR_CAMPUSSTRUCTS_H
#define COLLEGETOUR_CAMPUSSTRUCTS_H
#include <qlist.h>
#include <QString>

/**
 * @struct campusSouvenir
 * @brief Represents a single souvenir item available at a campus.
 */
struct campusSouvenir {
    QString name;   /**< The name of the souvenir item */
    double price;   /**< The cost of the souvenir in USD */
};

/**
 * @struct CampusDistances
 * @brief Stores the distance relationship between the parent campus and another campus.
 */
struct CampusDistances {
    int otherCampusID; /**< The campusID of the other campus */
    double distance;   /**< The travel distance between the two campuses */
};

/**
 * @struct Campus
 * @brief A comprehensive container for all data related to a single college campus.
 * * This struct aggregates the campus identity, its full inventory of souvenirs,
 * and its network of distances to other colleges.
 */
struct Campus {
    int campusID;                       /**< campusID for the campus */
    QString campusName;                 /**< The name of the college */
    QList<CampusDistances> distances;   /**< QList of distance connections to other campuses */
    QList<campusSouvenir> souvenirs;    /**< QList of all souvenirs available at this location */
};

#endif //COLLEGETOUR_CAMPUSSTRUCTS_H