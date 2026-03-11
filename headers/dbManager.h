
/**
 * @file dbManager.h
 * @brief Header file for the DbManager class, handling SQLite database connectivity.
 */
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
/**
 * @class DbManager
 * @brief Manages the connection, state, and management of the application's SQL database.
 * * This class handles the SQL connection.
 * * 
 * * @note To use signals/slots, ensure this class inherits from QObject.
 */
class DbManager {
public:
    // Constructor: takes path to .db file and initializes the database
    /**
     * @brief Constructor that initializes the database connection.
     * Opens a connection to the SQLite database file at the provided path.
     * @param path The filesystem path to the .db file.
     */
    DbManager(const QString& path);

    /**
     * @brief Destructor that safely closes the database connection.
     * Ensures all pending transactions are handled and the file handle is released.
     */
    ~DbManager();

    /**
     * @brief Validates if the database connection is currently active and valid.
     * @return true if the database is open and usable; false otherwise.
     */
    bool isOpen() const;

    // In dbManager.h
    /**
     * @brief Provides direct access to  QSqlDatabase object.
     * @return A copy of the QSqlDatabase instance.
     */
    QSqlDatabase getDatabase() const { return m_db; }

    /**
     * @brief Triggers the dataChanged() signal to notify observers.
     * Call this method after any INSERT, UPDATE, or DELETE operation to 
     * ensure the UI stays in sync.
     */
    void notifyDataChange() { emit dataChanged(); }

private:
    QSqlDatabase m_db; /**< The Qt SQL database connection object. */

signals:
/**
     * @brief Signal emitted whenever the database content is modified.
     * Connect this signal to your @ref DatabasePage::refreshUI slots to 
     * automate interface updates.
     */
    void dataChanged();
};

#endif // DBMANAGER_H
