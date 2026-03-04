#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <QString>
#include <QVector>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

struct TripResult {
    QVector<int> campusOrder;
    double totalDistance = 0.0;
};

class TripPlanner {
public:
    TripPlanner() {}

    // Agile Requirement: Recursive checker for closest campus
    void planRecursiveTrip(int currentId, QVector<int>& remainingIds, TripResult& result) {
        if (remainingIds.isEmpty()) return;

        double minDistance = 1e9; // Infinity
        int closestId = -1;
        int closestIndex = -1;

        // Find the next closest campus among those selected
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
            
            // Recursive call for the next step of the trip
            planRecursiveTrip(closestId, remainingIds, result);
        }
    }

private:
    // Agile Requirement: Distance tracker / Shortest distance calculator
    double getDistance(int startID, int endID) {
        if (startID == endID) return 0.0;
        
        QSqlQuery query;
        // Queries the 'distances' table mentioned in your databaseHelper
        query.prepare("SELECT distance FROM distances WHERE "
                      "(startID = :s AND endID = :e) OR (startID = :e AND endID = :s)");
        query.bindValue(":s", startID);
        query.bindValue(":e", endID);

        if (query.exec() && query.next()) {
            return query.value(0).toDouble();
        }
        return 9999.0; // Return penalty if data is missing
    }
};

#endif
