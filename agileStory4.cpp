#include <iostream>
#include <string>
#include <vector>
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

// distance matrix (miles)
double distances[11][11] = {
    {0,8,20,15,195,10,370,18,5,6,12},
    {8,0,14,18,200,12,365,16,7,10,14},
    {20,14,0,25,210,18,380,22,19,21,23},
    {15,18,25,0,205,16,375,10,12,14,9},
    {195,200,210,205,0,198,550,210,190,192,200},
    {10,12,18,16,198,0,360,15,8,9,13},
    {370,365,380,375,550,360,0,365,368,372,374},
    {18,16,22,10,210,15,365,0,13,15,7},
    {5,7,19,12,190,8,368,13,0,4,11},
    {6,10,21,14,192,9,372,15,4,0,10},
    {12,14,23,9,200,13,374,7,11,10,0}
};

// function to find closest campus
int find_closest(int current, vector<int> remaining) {

    double min_distance = numeric_limits<double>::max();
    int closest = -1;

    for (int i = 0; i < remaining.size(); i++) {

        double d = distances[current][remaining[i]];

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

    // find closest campus
    int next = find_closest(current, remaining);

    total_distance += distances[current][next];
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

    // display campuses
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
