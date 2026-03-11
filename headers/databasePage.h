/**
 * @file databasePage.h
 * @brief Base class for UI components that sync with the database.
 */
#ifndef DATABASEPAGE_H
#define DATABASEPAGE_H

#include <QWidget>



// This is the parent class for ANY page that needs to refresh when the DB changes
/**
 * @class DatabasePage
 * @brief An abstract base class for all pages that require real-time UI updates.
 * * This class serves as a template for any QWidget that must stay synchronized 
 * with the underlying database. It enforces a consistent refresh pattern 
 * across the application.
 * * 
 */
class DatabasePage : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Constructs a DatabasePage.
     * @param parent The parent widget, defaulting to nullptr.
     */
    explicit DatabasePage(QWidget *parent = nullptr) : QWidget(parent) {}
    /**
     * @brief Pure virtual function to refresh the user interface.
     * * Every child class MUST implement this function to reload data from the 
     * database and update its specific UI elements.
     * 
     */
    // Every child page MUST implement this function
    virtual void refreshUI() = 0;
};

#endif
