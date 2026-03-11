/**
 * @file campusStructs.h
 * @brief Core data structures representing campuses, souvenirs, and graph edges.
 * @author Erfan Tavassoli
 * @date 2026-02-25
 */

#ifndef COLLEGETOUR_CAMPUSSTRUCTS_H
#define COLLEGETOUR_CAMPUSSTRUCTS_H

#include <qlist.h>
#include <QString>

/**
 * @struct campusSouvenir
 * @brief Represents a single gift shop item available at a campus.
 */
struct campusSouvenir {
    QString name;   /**< Name of the souvenir item. */
    double price;   /**< Price in USD. */
};

/**
 * @struct CampusDistances
 * @brief Represents an edge in the campus graph.
 * @details This structure stores the weight (distance) between the parent campus and a neighbor.
 */
struct CampusDistances {
    int otherCampusID;  /**< ID of the destination campus. */
    double distance;    /**< Distance in miles between the two campuses. */
};

/**
 * @struct Campus
 * @brief Aggregate structure containing all data for a single university.
 * @details Combines basic identification with lists of neighboring nodes (graph edges) 
 * and available inventory items.
 */
struct Campus {
    int campusID;                       /**< Unique primary key from the database. */
    QString campusName;                 /**< Formal name of the university. */
    QList<CampusDistances> distances;   /**< Adjacency list of neighboring campuses. */
    QList<campusSouvenir> souvenirs;    /**< List of all souvenirs sold at this location. */
};

#endif //COLLEGETOUR_CAMPUSSTRUCTS_H
