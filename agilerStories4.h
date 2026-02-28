#ifndef CAMPUS_MANAGER_H
#define CAMPUS_MANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>

// Represents a campus row from the database 
struct Campus {
    int id;
    std::string name;
    double latitude;
    double longitude;
};

class CampusManager {
public:
    // Opens the test.db file 
    CampusManager(const std::string& dbPath);
    ~CampusManager();

    // Fetches all campuses including their coordinates from the DB
    std::vector<Campus> getAllCampuses();

private:
    sqlite3* db;
    // Callback for SQLite execution
    static int callback(void* data, int argc, char** argv, char** azColName);
};

#endif
