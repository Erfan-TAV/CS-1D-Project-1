#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

// structure to store college information
struct College {
    int id;
    string name;
    bool visited;
};

// class to manage college database
class CollegeDatabase {
private:
    vector<College> colleges;
    vector<vector<double>> distanceMatrix;
    double totalDistance;

    // recursive function to build most efficient path
    void buildPathRecursive(int currentIndex, int visitedCount) {
        if (visitedCount == colleges.size()) {
            return;
        }

        double minDist = numeric_limits<double>::max();
        int nextIndex = -1;

        // find closest unvisited campus
        for (int i = 0; i < colleges.size(); i++) {
            if (!colleges[i].visited) {
                double dist = distanceMatrix[currentIndex][i];

                if (dist < minDist) {
                    minDist = dist;
                    nextIndex = i;
                }
            }
        }

        // visit next campus
        if (nextIndex != -1) {
            colleges[nextIndex].visited = true;
            totalDistance += minDist;

            cout << " -> [" << colleges[nextIndex].id << "] "
                 << colleges[nextIndex].name
                 << " (" << minDist << " mi)";

            buildPathRecursive(nextIndex, visitedCount + 1);
        }
    }

public:
    // constructor
    CollegeDatabase() {
        totalDistance = 0.0;

        // initial 11 campuses (correct list)
        colleges.push_back({1, "Saddleback College", false});
        colleges.push_back({2, "University of California, Irvine (UCI)", false});
        colleges.push_back({3, "University of California, Los Angeles (UCLA)", false});
        colleges.push_back({4, "University of the Pacific", false});
        colleges.push_back({5, "University of Oregon", false});
        colleges.push_back({6, "Arizona State University", false});
        colleges.push_back({7, "Ohio State University", false});
        colleges.push_back({8, "University of Michigan", false});
        colleges.push_back({9, "Northwestern", false});
        colleges.push_back({10, "University of Wisconsin", false});
        colleges.push_back({11, "Massachusetts Institute of Technology (MIT)", false});

        initializeDistanceMatrix();
    }

    // initialize official distance matrix
    void initializeDistanceMatrix() {
        distanceMatrix = {
        // SB, UCI, UCLA, UOP, OREG, ASU, OSU, MICH, NW, WISC, MIT
        {0,   16,   66.3, 390, 906, 379, 2252, 2249, 2033, 1981, 2989}, // SB
        {16,  0,    51.5, 383, 899, 375, 2248, 2245, 2030, 1978, 2986}, // UCI
        {66.3,51.5, 0,    333, 849, 397, 2261, 2252, 2043, 1991, 2998}, // UCLA
        {390, 383,  333,  0,   517, 720, 2412, 2318, 2102, 2052, 3057}, // UOP
        {906, 899,  849,  517, 0,   1343,2241, 2347, 2151, 2111, 3107}, // OREG
        {379, 375,  397,  720, 1343,0,   1865, 1956, 1756, 1684, 2636}, // ASU
        {2252,2248,2261,2412,2241,1865,0,   184,  335,  503,  772},  // OSU
        {2249,2245,2252,2318,2347,1956,184, 0,    270,  391,  803},  // MICH
        {2033,2030,2043,2102,2151,1756,335, 270,  0,    145,  993},  // NW
        {1981,1978,1991,2052,2111,1684,503, 391,  145,  0,    1125}, // WISC
        {2989,2986,2998,3057,3107,2636,772, 803,  993,  1125, 0}     // MIT
        };
    }

    // display the most efficient trip
    void displayTrip() {
        // reset visited flags
        for (int i = 0; i < colleges.size(); i++) {
            colleges[i].visited = false;
        }

        totalDistance = 0.0;

        cout << fixed << setprecision(1);
        cout << "\n===== Most Efficient Campus Trip =====\n";

        // start at Saddleback
        colleges[0].visited = true;
        cout << "[" << colleges[0].id << "] "
             << colleges[0].name;

        buildPathRecursive(0, 1);

        cout << "\n\nTotal Distance Traveled: "
             << totalDistance << " miles\n";
    }
};

int main() {
    CollegeDatabase db;
    int option;

    while (true) {
        cout << "\n===== Campus Trip Planner =====\n";
        cout << "1. Show Most Efficient Trip\n";
        cout << "2. Exit\n";
        cout << "Enter option: ";
        cin >> option;

        if (option == 1) {
            db.displayTrip();
        }
        else if (option == 2) {
            break;
        }
        else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}
