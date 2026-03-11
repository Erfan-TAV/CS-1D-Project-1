/**
 * @file dbManager.h
 * @brief Core database controller for managing the SQLite connection and lifecycle.
 * @author Erfan Tavassoli
 */

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QFile>

/**
 * @class DbManager
 * @brief Manages the physical connection to the SQLite database.
 * @details This class handles the initialization of the database file. If the database 
 * does not exist in the local writable directory, it copies a template from the 
 * application resources, ensures write permissions are set, and opens the connection.
 */
class DbManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor that initializes and opens the SQLite database.
     * @details The constructor performs the following sequence:
     * 1. Extracts the directory path from the provided fullPath.
     * 2. Creates the target directory if it does not exist.
     * 3. Checks if the .db file exists at fullPath; if not, copies it from ':/res/project1.db'.
     * 4. Adjusts file permissions to ensure the database is writable.
     * 5. Opens the connection using the QSQLITE driver.
     * @param fullPath The target absolute path where the database should reside.
     */
    DbManager(const QString& fullPath);

    /**
     * @brief Destructor that safely closes the database connection.
     */
    ~DbManager();

    /**
     * @brief Validates if the database connection is currently active.
     * @return true if the database is open and ready for queries.
     */
    bool isOpen() const;

    /**
     * @brief Returns a handle to the internal QSqlDatabase object.
     * @return The active QSqlDatabase instance for executing queries.
     */
    QSqlDatabase getDatabase() const { return m_db; }

    /**
     * @brief Manually triggers the dataChanged() signal.
     * @details Emits the dataChanged() signal to notify any connected UI pages 
     * (DatabasePage objects) that they need to refresh their views.
     */
    void notifyDataChange() { emit dataChanged(); }

private:
    QSqlDatabase m_db; /**< The underlying Qt SQL database object. */

signals:
    /**
     * @brief Signal emitted whenever the database content is modified.
     * @details This allows for real-time UI updates across different tabs 
     * when the underlying data changes.
     */
    void dataChanged();
};

#endif // DBMANAGER_H
