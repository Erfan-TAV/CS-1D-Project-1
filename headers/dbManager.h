#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlQuery>

class DbManager {
public:
    // Constructor: takes path to .db file and initializes the database
    DbManager(const QString& path);

    // Destructor: closes the file connection
    ~DbManager();

    // Check if the database is actually open
    bool isOpen() const;

    // In dbManager.h
    QSqlDatabase getDatabase() const { return m_db; }

    void notifyDataChange() { emit dataChanged(); }

private:
    QSqlDatabase m_db;

signals:
    void dataChanged();
};

#endif // DBMANAGER_H
