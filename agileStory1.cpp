#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

// structure to store college information
struct College {
    string name;
    double latitude;
    double longitude;
    double distance;
    double x; // x-coordinate (east-west)
    double y; // y-coordinate (north-south)
};

// class to manage college database
class CollegeDatabase {
private:
    vector<College> colleges;

public:
    // constructor to initialize sample colleges
    CollegeDatabase() {
        colleges.push_back({"UCLA", 34.0689, -118.4452, 0, 0, 0});
        colleges.push_back({"UC Irvine", 33.6405, -117.8443, 0, 0, 0});
        colleges.push_back({"Cal State Fullerton", 33.8823, -117.8851, 0, 0, 0});
        colleges.push_back({"USC", 34.0224, -118.2851, 0, 0, 0});
        colleges.push_back({"San Diego State", 32.7757, -117.0719, 0, 0, 0});
        colleges.push_back({"Stanford University", 37.4275, -122.1697, 0, 0, 0});
        colleges.push_back({"UC Berkeley", 37.8715, -122.2730, 0, 0, 0});
        colleges.push_back({"Cal Poly Pomona", 34.0572, -117.8216, 0, 0, 0});

        compute_distances();
    }

    // function to compute distances and coordinates relative to Saddleback College
    void compute_distances() {
        double saddleback_lat = 33.5514;
        double saddleback_lon = -117.6626;

        for (int i = 0; i < colleges.size(); i++) {
            double delta_lat = colleges[i].latitude - saddleback_lat;
            double delta_lon = colleges[i].longitude - saddleback_lon;

            // convert degrees to miles approximately
            double miles_per_deg_lat = 69.0;
            double miles_per_deg_lon = cos(saddleback_lat * M_PI / 180.0) * 69.0;

            colleges[i].x = delta_lon * miles_per_deg_lon;
            colleges[i].y = delta_lat * miles_per_deg_lat;

            colleges[i].distance = sqrt(colleges[i].x * colleges[i].x +
                                        colleges[i].y * colleges[i].y);
        }
    }

    // function to display colleges (scrollable by pages)
    void display_colleges() {
        int page = 0;
        int page_size = 5;
        char choice;

        while (true) {
            int start = page * page_size;
            int end = start + page_size;

            if (start >= colleges.size()) {
                cout << "No more colleges.\n";
                page--;
                continue;
            }

            cout << "\n--- College List (Page " << page + 1 << ") ---\n";

            for (int i = start; i < end && i < colleges.size(); i++) {
                cout << i + 1 << ". "
                     << colleges[i].name << " - "
                     << fixed << setprecision(2)
                     << colleges[i].distance
                     << " miles "
                     << "(x: " << fixed << setprecision(2) << colleges[i].x
                     << ", y: " << colleges[i].y << ")" << endl;
            }

            cout << "\nn - next page | p - previous page | s - select | q - quit\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 'n') {
                page++;
            }
            else if (choice == 'p' && page > 0) {
                page--;
            }
            else if (choice == 's') {
                int num;
                cout << "Enter college number: ";
                cin >> num;

                if (num > 0 && num <= colleges.size()) {
                    cout << "\nYou selected: "
                         << colleges[num - 1].name << endl;
                    cout << "Distance: "
                         << fixed << setprecision(2)
                         << colleges[num - 1].distance
                         << " miles "
                         << "(x: " << colleges[num - 1].x
                         << ", y: " << colleges[num - 1].y << ")" << endl;
                }
                else {
                    cout << "Invalid selection.\n";
                }
            }
            else if (choice == 'q') {
                break;
            }
        }
    }

    // function to search for a college
    void search_college() {
        string search_name;
        bool found = false;

        cout << "Enter college name to search: ";
        cin.ignore();
        getline(cin, search_name);

        for (int i = 0; i < colleges.size(); i++) {
            if (colleges[i].name.find(search_name) != string::npos) {
                cout << colleges[i].name << " - "
                     << fixed << setprecision(2)
                     << colleges[i].distance
                     << " miles "
                     << "(x: " << colleges[i].x
                     << ", y: " << colleges[i].y << ")" << endl;
                found = true;
            }
        }

        if (!found) {
            cout << "College not found.\n";
        }
    }
};

int main() {
    CollegeDatabase db;
    int option;

    while (true) {
        cout << "\n===== College Distance Viewer =====\n";
        cout << "1. View Colleges\n";
        cout << "2. Search College\n";
        cout << "3. Exit\n";
        cout << "Enter option: ";
        cin >> option;

        if (option == 1) {
            db.display_colleges();
        }
        else if (option == 2) {
            db.search_college();
        }
        else if (option == 3) {
            break;
        }
        else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}
