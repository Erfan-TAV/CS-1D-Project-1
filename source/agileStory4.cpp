#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;

// list of 11 ASU campuses
vector<string> campuses = {
    "Tempe",
    "Downtown Phoenix",
    "West",
    "Polytechnic",
    "Lake Havasu",
    "Thunderbird",
    "California Center",
    "Mesa City Center",
    "SkySong",
    "Research Park",
    "Chandler Innovation Center"
};

// coordinates (latitude, longitude)
double coordinates[11][2] = {
    {33.4242, -111.9281},  // Tempe
    {33.4534, -112.0738},  // Downtown Phoenix
    {33.6073, -112.1590},  // West
    {33.3078, -111.6824},  // Polytechnic
    {34.4839, -114.3225},  // Lake Havasu
    {33.4942, -112.0566},  // Thunderbird
    {34.0689, -118.4452},  // California Center
    {33.4152, -111.8315},  // Mesa City Center
    {33.4326, -111.8906},  // SkySong
    {33.2867, -111.7340},  // Research Park
    {33.3062, -111.8413}   // Chandler Innovation Center
};

// function to calculate distance between two campuses
double calculate_distance(int a, int b) {

    double lat1 = coordinates[a][0];
    double lon1 = coordinates[a][1];
    double lat2 = coordinates[b][0];
    double lon2 = coordinates[b][1];

    double distance = sqrt(pow(lat2 - lat1, 2) +
                           pow(lon2 - lon1, 2));

    // convert degrees difference roughly to miles
    return distance * 69.0;
}

// function to find closest campus
int find_closest(int current, vector<int> remaining) {

    double min_distance = numeric_limits<double>::max();
    int closest = -1;

    for (int i = 0; i < remaining.size(); i++){

        double d = calculate_distance(current, remaining[i]);

        if (d < min_distance){
            min_distance = d;
            closest = remaining[i];
        }
    }

    return closest;
}

// recursive function to visit campuses
void plan_trip(int current,
               vector<int> remaining,
               vector<int>& order,
               double& total_distance) {

    // base case
    if (remaining.size() == 0){
        return;
    }

    int next = find_closest(current, remaining);

    total_distance += calculate_distance(current, next);
    order.push_back(next);

    // remove visited campus
    vector<int> new_remaining;

    for (int i = 0; i < remaining.size(); i++){
        if (remaining[i] != next){
            new_remaining.push_back(remaining[i]);
        }
    }

    // recursive call
    plan_trip(next, new_remaining, order, total_distance);
}

int main() {

    int start;
    int count;

    cout << "ASU Recursive Campus Trip Planner" << endl << endl;

    for (int i = 0; i < campuses.size(); i++){
        cout << i << ": " << campuses[i] << endl;
    }

    cout << endl;
    cout << "Enter starting campus number: ";
    cin >> start;

    cout << "How many campuses do you want to visit (including start)? ";
    cin >> count;

    vector<int> selected;
    selected.push_back(start);

    cout << "Enter the campus numbers you want to include:" << endl;

    for (int i = 1; i < count; i++){

        int choice;
        cin >> choice;

        // prevent starting campus from being selected again
        while (choice == start){
            cout << "Error: Starting campus already selected." << endl;
            cout << "Please enter a different campus: ";
            cin >> choice;
        }

        selected.push_back(choice);
    }

    vector<int> remaining;

    for (int i = 1; i < selected.size(); i++){
        remaining.push_back(selected[i]);
    }

    vector<int> trip_order;
    trip_order.push_back(start);

    double total_distance = 0;

    // recursive call
    plan_trip(start, remaining, trip_order, total_distance);

    cout << endl;
    cout << "Trip Order:" << endl;

    for (int i = 0; i < trip_order.size(); i++){
        cout << trip_order[i] << ": "
             << campuses[trip_order[i]] << endl;
    }

    cout << endl;
    cout << "Total Distance: "
         << total_distance << " miles" << endl;

    return 0;
}
