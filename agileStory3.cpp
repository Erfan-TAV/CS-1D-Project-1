#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <limits>

using namespace std;

// structure to store college information
struct College {
    string name;
    double latitude;
    double longitude;
    double distance;
    double x; // x-coordinate (east-west)
    double y; // y-coordinate (north-south)
    bool visited;
};

// class to manage college database
class CollegeDatabase {
private:
    vector<College> colleges;
    double total_distance;

    // recursive function to build most efficient path
    void build_path_recursive(int current_index, int visited_count) {
        if (visited_count == colleges.size()) {
            return;
        }

        double min_dist = numeric_limits<double>::max();
        int next_index = -1;

        for (int i = 0; i < colleges.size(); i++) {
            if (!colleges[i].visited) {
                double dx = colleges[i].x - colleges[current_index].x;
                double dy = colleges[i].y - colleges[current_index].y;
                double dist = sqrt(dx * dx + dy * dy);

                if (dist < min_dist) {
                    min_dist = dist;
                    next_index = i;
                }
            }
        }

        if (next_index != -1) {
            colleges[next_index].visited = true;
            total_distance += min_dist;

            cout << " -> " << colleges[next_index].name;
            build_path_recursive(next_index, visited_count + 1);
        }
    }

public:
    // constructor to initialize the initial 11 campuses
    CollegeDatabase() {
        total_distance = 0.0;

        colleges.push_back({"Saddleback College", 33.5514, -117.6626, 0, 0, 0, false});
        colleges.push_back({"UC Irvine", 33.6405, -117.8443, 0, 0, 0, false});
        colleges.push_back({"Cal State Fullerton", 33.8823, -117.8851, 0, 0, 0, false});
        colleges.push_back({"Long Beach State", 33.7838, -118.1141, 0, 0, 0, false});
        colleges.push_back({"UCLA", 34.0689, -118.4452, 0, 0, 0, false});
        colleges.push_back({"USC", 34.0224, -118.2851, 0, 0, 0, false});
        colleges.push_back({"Cal Poly Pomona", 34.0572, -117.8216, 0, 0, 0, false});
        colleges.push_back({"Mt. SAC", 34.0500, -117.8216, 0, 0, 0, false});
        colleges.push_back({"Riverside City College", 33.9737, -117.3281, 0, 0, 0, false});
        colleges.push_back({"UC Riverside", 33.9737, -117.3281, 0, 0, 0, false});
        colleges.push_back({"Chapman University", 33.7933, -117.8516, 0, 0, 0, false});

        compute_distances();
    }

    // compute coordinates relative to Saddleback
    void compute_distances() {
        double saddle_lat = colleges[0].latitude;
        double saddle_lon = colleges[0].longitude;

        for (int i = 0; i < colleges.size(); i++) {
            double delta_lat = colleges[i].latitude - saddle_lat;
            double delta_lon = colleges[i].longitude - saddle_lon;

            double miles_per_deg_lat = 69.0;
            double miles_per_deg_lon = cos(saddle_lat * M_PI / 180.0) * 69.0;

            colleges[i].x = delta_lon * miles_per_deg_lon;
            colleges[i].y = delta_lat * miles_per_deg_lat;

            colleges[i].distance = sqrt(colleges[i].x * colleges[i].x +
                                        colleges[i].y * colleges[i].y);

            colleges[i].visited = false;
        }
    }

    // display the most efficient trip
    void display_trip() {
        // reset visited flags
        for (int i = 0; i < colleges.size(); i++) {
            colleges[i].visited = false;
        }

        total_distance = 0.0;

        cout << fixed << setprecision(2);
        cout << "\n===== Most Efficient Campus Trip =====\n";

        // start at Saddleback (index 0)
        colleges[0].visited = true;
        cout << colleges[0].name;

        build_path_recursive(0, 1);

        cout << "\n\nTotal Distance Traveled: "
             << total_distance << " miles\n";
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
            db.display_trip();
        }
        else if (option == 32) {
            break;
        }
        else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}
