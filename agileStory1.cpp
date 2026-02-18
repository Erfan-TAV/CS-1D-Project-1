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
};

// function to calculate distance using Haversine formula
double calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    const double EARTH_RADIUS = 3958.8; // miles

    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = pow(sin(dLat / 2), 2) +
               pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);

    double c = 2 * asin(sqrt(a));

    return EARTH_RADIUS * c;
}

// function to compute distances from Saddleback College
void compute_distances(vector<College>& colleges) {

    double saddleback_lat = 33.5514;
    double saddleback_lon = -117.6626;

    for (int i = 0; i < colleges.size(); i++) {
        colleges[i].distance = calculate_distance(
            saddleback_lat,
            saddleback_lon,
            colleges[i].latitude,
            colleges[i].longitude
        );
    }
}

// function to display colleges (scrollable by pages)
void display_colleges(vector<College>& colleges) {

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
                 << " miles" << endl;
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
                     << " miles\n";
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
void search_college(vector<College>& colleges) {

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
                 << " miles" << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "College not found.\n";
    }
}

int main() {

    vector<College> colleges;

    // sample database of colleges
    colleges.push_back({"UCLA", 34.0689, -118.4452, 0});
    colleges.push_back({"UC Irvine", 33.6405, -117.8443, 0});
    colleges.push_back({"Cal State Fullerton", 33.8823, -117.8851, 0});
    colleges.push_back({"USC", 34.0224, -118.2851, 0});
    colleges.push_back({"San Diego State", 32.7757, -117.0719, 0});
    colleges.push_back({"Stanford University", 37.4275, -122.1697, 0});
    colleges.push_back({"UC Berkeley", 37.8715, -122.2730, 0});
    colleges.push_back({"Cal Poly Pomona", 34.0572, -117.8216, 0});

    compute_distances(colleges);

    int option;

    // main menu loop
    while (true) {

        cout << "\n===== College Distance Viewer =====\n";
        cout << "1. View Colleges\n";
        cout << "2. Search College\n";
        cout << "3. Exit\n";
        cout << "Enter option: ";
        cin >> option;

        if (option == 1) {
            display_colleges(colleges);
        }
        else if (option == 2) {
            search_college(colleges);
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
