#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <QString>
#include <QVector>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <cmath>

// Data structure to store trip results
struct TripResult {
    QVector<int> campusOrder;
    double totalDistance;
};

class TripPlanner {
public:
    TripPlanner() {}

    /**
     * @brief The core recursive function to find the shortest trip.
     * Uses a "Nearest Neighbor" approach to ensure a shorter path.
     */
    void planShortestTrip(int currentId, 
                          QVector<int>& remainingIds, 
                          TripResult& result) {
        
        // Base Case: No more campuses to visit
        if (remainingIds.isEmpty()) return;

        double minDistance = 1e9; // Start with a very high number
        int closestId = -1;
        int closestIndex = -1;

        // Loop through remaining campuses to find the closest one
        for (int i = 0; i < remainingIds.size(); ++i) {
            double d = fetchDistance(currentId, remainingIds[i]);
            if (d < minDistance) {
                minDistance = d;
                closestId = remainingIds[i];
                closestIndex = i;
            }
        }

        if (closestId != -1) {
            // Update the running totals
            result.totalDistance += minDistance;
            result.campusOrder.append(closestId);
            
            // Remove the visited campus from the list
            remainingIds.removeAt(closestIndex);
            
            // Recurse to find the next closest campus
            planShortestTrip(closestId, remainingIds, result);
        }
    }

    /**
     * @brief Fetches distance from the 'distances' table in project1.db
     */
    double fetchDistance(int startID, int endID) {
        if (startID == endID) return 0.0;
        
        QSqlQuery query;
        // Search both ways in case the database only stores (A to B) and not (B to A)
        query.prepare("SELECT distance FROM distances WHERE "
                      "(startID = :start AND endID = :end) OR "
                      "(startID = :end AND endID = :start)");
        query.bindValue(":start", startID);
        query.bindValue(":end", endID);

        if (query.exec() && query.next()) {
            return query.value(0).toDouble();
        }
        
        qDebug() << "Distance not found for IDs:" << startID << endID;
        return 999.9; // Penalty distance if missing
    }
};

#endif // TRIPPLANNER_H
