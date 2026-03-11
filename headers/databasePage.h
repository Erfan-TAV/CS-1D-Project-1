/**
 * @file databasePage.h
 * @brief Base class for UI pages requiring database synchronization.
 * @author Erfan Tavassoli
 */

#ifndef DATABASEPAGE_H
#define DATABASEPAGE_H

#include <QWidget>

/**
 * @class DatabasePage
 * @brief An abstract base class for all pages that display database-driven content.
 * @details This class serves as an interface to ensure that all sub-pages 
 * (like Admin, View Campuses, etc.) implement a standardized refresh mechanism 
 * to stay in sync with the SQLite database.
 */
class DatabasePage : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Constructor for DatabasePage.
     * @param parent Pointer to the parent QWidget.
     */
    explicit DatabasePage(QWidget *parent = nullptr) : QWidget(parent) {}

    /**
     * @brief Pure virtual function to update the UI with the latest database data.
     * @details Every child class must implement this method to reload its models, 
     * refresh table views, or update labels whenever a change occurs in the database.
     */
    virtual void refreshUI() = 0;
};

#endif // DATABASEPAGE_H
