//
// Created by erfan on 2/25/2026.
//

#ifndef COLLEGETOUR_CAMPUSSTRUCTS_H
#define COLLEGETOUR_CAMPUSSTRUCTS_H
#include <qlist.h>
#include <QString>

struct campusSouvenir {
    QString name;
    double price;
};

struct CampusDistances {
    int otherCampusID;
    double distance;
};

struct Campus {
    int campusID;
    QString campusName;
    QList<CampusDistances> distances;
    QList<campusSouvenir> souvenirs;
};



#endif //COLLEGETOUR_CAMPUSSTRUCTS_H