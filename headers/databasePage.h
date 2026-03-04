#ifndef DATABASEPAGE_H
#define DATABASEPAGE_H

#include <QWidget>

// This is the parent class for ANY page that needs to refresh when the DB changes
class DatabasePage : public QWidget {
    Q_OBJECT
public:
    explicit DatabasePage(QWidget *parent = nullptr) : QWidget(parent) {}

    // Every child page MUST implement this function
    virtual void refreshUI() = 0;
};

#endif
