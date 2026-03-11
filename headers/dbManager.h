/**
 * @file dbManager.h
 * @brief Core database controller for managing the SQLite connection.
 * @author Erfan Tavassoli
 */

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QObject>

/**
 * @class DbManager
 * @brief Manages the physical connection to the SQLite database.
 * @details This class initializes the database connection, ensures it remains open,
 * and provides a signaling mechanism to notify the rest of the application
 * whenever the data is modified (e.g., after an admin upload).
 */
class DbManager : public QObject {
    Q_OBJECT  /**< Required for Signal/Slot functionality */

public:
    /**
     * @brief Constructor that opens or creates the SQLite database.
     * @param path The file path to the .db file.
     */
    DbManager(const QString& path);

    /**
     * @brief Destructor that safely closes the database connection.
     */
    ~DbManager();

    /**
     * @brief Validates the current state of the database connection.
     * @return true if the database is open and ready for queries.
     */
    bool isOpen() const;

    /**
     * @brief Returns a handle to the internal QSqlDatabase object.
     * @return The active QSqlDatabase instance.
     */
    QSqlDatabase getDatabase() const { return m_db; }

    /**
     * @brief Manually triggers the dataChanged() signal.
     * @details Call this after performing batch operations (like Excel imports)
     * to force UI elements to refresh their views.
     */
    void notifyDataChange();

private:
    QSqlDatabase m_db; /**< The underlying Qt SQL database object. */

signals:
    /**
     * @brief Signal emitted whenever the database content is modified.
     * @details Connect this signal to the refreshUI() slots of your DatabasePage objects.
     */
    void dataChanged();
};

#endif // DBMANAGER_H
