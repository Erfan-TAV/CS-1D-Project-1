#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <QVector>
#include <QSqlQuery>
#include <QString>

struct TripResult {
    QVector<int> campusOrder;
    double totalDistance = 0.0;
};

class tripPlanner {
public:
    tripPlanner() {}

    /**
     * @brief Core Recursive Algorithm
     * Finds the next closest campus among the targets to ensure efficiency.
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
