#pragma once
#include <string>
#include <vector>

struct Souvenir {
    std::string name;
    double price;
};

class CollegeManager {
public:
    CollegeManager();

    int collegeCount() const;
    const std::string& collegeName(int i) const;

    int saddleIndex() const;
    int currentIndex() const;

    bool isVisited(int i) const;
    std::vector<int> availableColleges() const;

    double distance(int from, int to) const;
    int closestOfferIndex() const;            // closest unvisited from current (or -1)

    void visitCollege(int chosenIndex);       // adds current->chosen to miles, marks visited

    const std::vector<Souvenir>& souvenirsFor(int collegeIndex) const;

    void buySouvenir(int collegeIndex, int souvenirIndex, int quantity);

    double totalMiles() const;
    double grandTotal() const;
    double spentAtCollege(int collegeIndex) const;

private:
    void buildData();
    void setD(int a, int b, double d);

    std::vector<std::string> m_colleges;
    std::vector<std::vector<double>> m_dist;
    std::vector<std::vector<Souvenir>> m_souvenirs;

    std::vector<bool> m_visited;
    std::vector<double> m_spentPerCollege;

    int m_saddleIndex;
    int m_currentIndex;
    double m_totalMiles;
    double m_grandTotal;
};

