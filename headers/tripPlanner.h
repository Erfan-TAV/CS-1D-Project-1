/**
 * @file tripPlanner.h
 * @brief Header and implementation of the trip planning logic.
 */
#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <QVector>
#include <QSqlQuery>
#include <QString>
#include <QDebug>

/**
 * @struct TripResult
 * @brief Holds the outcome of a calculated trip.
 */
struct TripResult {
    QVector<int> campusOrder; /**< The sequence of campus IDs visited in order. */
    double totalDistance = 0.0; /**< The cumulative distance of the entire trip in miles. */
};

/**
 * @class tripPlanner
 * @brief Logic class responsible for calculating optimized travel routes.
 * * Goal is to find an efficient path through a list of target campuses.
 * * 
 */
class tripPlanner {
public:
/**
     * @brief Default constructor for the tripPlanner.
     */
    tripPlanner() {}

   /**
     * @brief Core Recursive Algorithm to find the most efficient route.
     * * This function implements a recursive nearest neighbor search. From the current 
     * campus, it searches the `remainingIds` for the closest neighbor, adds it to 
     * the result, and recurses until no campuses remain.
     * * @param currentId The ID of the campus currently being visited.
     * @param remainingIds A list of campus IDs that have not been visited yet. 
     * Note: This list is modified during recursion.
     * @param result A reference to a @ref TripResult object where the path and 
     * total distance are stored.
     */
    void planRecursiveTrip(int currentId, QVector<int>& remainingIds, TripResult& result) {
        if (remainingIds.isEmpty()) return;

        double minDistance = 1e9; // Start with infinity
        int closestId = -1;
        int closestIndex = -1;

        for (int i = 0; i < remainingIds.size(); ++i) {
            double d = getDistance(currentId, remainingIds[i]);
            if (d < minDistance) {
                minDistance = d;
                closestId = remainingIds[i];
                closestIndex = i;
            }
        }

        if (closestId != -1) {
            result.totalDistance += minDistance;
            result.campusOrder.append(closestId);
            remainingIds.removeAt(closestIndex);
            
            // Recurse to the next stop
            planRecursiveTrip(closestId, remainingIds, result);
        }
    }

    /**
     * @brief Queries the database for the distance between two specific campuses.
     * * Searches the 'distances' table for a match between the two provided IDs.
     * * @param startID The ID of the origin campus.
     * @param endID The ID of the destination campus.
     * @return The distance as a double; returns 999.0 if no connection is found 
     * in the database.
     */
    double getDistance(int startID, int endID) {
            if (startID == endID) return 0.0;
            QSqlQuery query;
            query.prepare("SELECT distance FROM distances WHERE "
                          "(startID = :s AND endID = :e) OR (startID = :e AND endID = :s)");
            query.bindValue(":s", startID);
            query.bindValue(":e", endID);
            if (query.exec() && query.next()) return query.value(0).toDouble();
            return 999.0;
        }
};

#endif
