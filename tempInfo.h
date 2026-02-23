#ifndef TEMPINFO_H
#define TEMPINFO_H

#include <QString>
#include <QList>

struct Souvenir {
    QString name;
    double price;
};

struct Campus {
    QString name;
    QList<Souvenir> items;
};

#endif // TEMPINFO_H
