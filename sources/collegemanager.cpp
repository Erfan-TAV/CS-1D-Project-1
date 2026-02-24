#include "../headers/collegemanager.h"
#include <limits>

CollegeManager::CollegeManager()
{
    buildData();
}

int CollegeManager::collegeCount() const { return (int)m_colleges.size(); }
const std::string& CollegeManager::collegeName(int i) const { return m_colleges[i]; }

int CollegeManager::saddleIndex() const { return m_saddleIndex; }
int CollegeManager::currentIndex() const { return m_currentIndex; }

bool CollegeManager::isVisited(int i) const { return m_visited[i]; }

std::vector<int> CollegeManager::availableColleges() const
{
    std::vector<int> out;
    for (int i = 0; i < collegeCount(); i++) {
        if (!m_visited[i]) out.push_back(i);
    }
    return out;
}

double CollegeManager::distance(int from, int to) const { return m_dist[from][to]; }

int CollegeManager::closestOfferIndex() const
{
    int best = -1;
    double bestD = std::numeric_limits<double>::max();

    for (int i = 0; i < collegeCount(); i++) {
        if (m_visited[i]) continue;
        if (i == m_currentIndex) continue;

        double d = m_dist[m_currentIndex][i];
        if (d < bestD) {
            bestD = d;
            best = i;
        }
    }
    return best;
}

void CollegeManager::visitCollege(int chosenIndex)
{
    m_totalMiles += m_dist[m_currentIndex][chosenIndex];
    m_currentIndex = chosenIndex;
    m_visited[chosenIndex] = true;
}

const std::vector<Souvenir>& CollegeManager::souvenirsFor(int collegeIndex) const
{
    return m_souvenirs[collegeIndex];
}

void CollegeManager::buySouvenir(int collegeIndex, int souvenirIndex, int quantity)
{
    if (quantity <= 0) return;
    double cost = m_souvenirs[collegeIndex][souvenirIndex].price * quantity;

    m_spentPerCollege[collegeIndex] += cost;
    m_grandTotal += cost;
}

double CollegeManager::totalMiles() const { return m_totalMiles; }
double CollegeManager::grandTotal() const { return m_grandTotal; }
double CollegeManager::spentAtCollege(int collegeIndex) const { return m_spentPerCollege[collegeIndex]; }

void CollegeManager::setD(int a, int b, double d)
{
    m_dist[a][b] = d;
    m_dist[b][a] = d;
}

void CollegeManager::buildData()
{
    // 11 colleges (Saddleback first)
    m_colleges = {
        "Saddleback College",                              // 0
        "University of California, Irvine (UCI)",           // 1
        "Massachusetts Institute of Technology (MIT)",      // 2
        "Arizona State University",                         // 3
        "Northwestern",                                     // 4
        "Ohio State University",                            // 5
        "University of Michigan",                           // 6
        "University of California, Los Angeles (UCLA)",     // 7
        "University of Oregon",                             // 8
        "University of the Pacific",                        // 9
        "University of Wisconsin"                           // 10
    };

    int N = (int)m_colleges.size();
    m_dist.assign(N, std::vector<double>(N, 0.0));

    // Saddleback (0)
    setD(0, 1, 16);
    setD(0, 2, 2989);
    setD(0, 3, 379);
    setD(0, 4, 2033);
    setD(0, 5, 2252);
    setD(0, 6, 2249);
    setD(0, 7, 66);
    setD(0, 8, 906);
    setD(0, 9, 390);
    setD(0, 10, 1981);

    // UCI (1)
    setD(1, 2, 2986);
    setD(1, 3, 375);
    setD(1, 4, 2030);
    setD(1, 5, 2248);
    setD(1, 6, 2245);
    setD(1, 7, 51);
    setD(1, 8, 899);
    setD(1, 9, 383);
    setD(1, 10, 1978);

    // MIT (2)
    setD(2, 3, 2636);
    setD(2, 4, 993);
    setD(2, 5, 772);
    setD(2, 6, 803);
    setD(2, 7, 2998);
    setD(2, 8, 3107);
    setD(2, 9, 3057);
    setD(2, 10, 1125);

    // ASU (3)
    setD(3, 4, 1756);
    setD(3, 5, 1865);
    setD(3, 6, 1956);
    setD(3, 7, 397);
    setD(3, 8, 1343);
    setD(3, 9, 720);
    setD(3, 10, 1684);

    // Northwestern (4)
    setD(4, 5, 335);
    setD(4, 6, 270);
    setD(4, 7, 2043);
    setD(4, 8, 2151);
    setD(4, 9, 2102);
    setD(4, 10, 145);

    // Ohio State (5)
    setD(5, 6, 184);
    setD(5, 7, 2261);
    setD(5, 8, 2241);
    setD(5, 9, 2412);
    setD(5, 10, 503);

    // Michigan (6)
    setD(6, 7, 2252);
    setD(6, 8, 2347);
    setD(6, 9, 2318);
    setD(6, 10, 391);

    // UCLA (7)
    setD(7, 8, 849);
    setD(7, 9, 333);
    setD(7, 10, 1991);

    // Oregon (8)
    setD(8, 9, 517);
    setD(8, 10, 2111);

    // Pacific (9)
    setD(9, 10, 2052);

    // Souvenirs: 3 items each (placeholder)
    m_souvenirs.assign(N, {});
    for (int i = 0; i < N; i++) {
        m_souvenirs[i].push_back({ "T-Shirt", 25.00 });
        m_souvenirs[i].push_back({ "Mug", 12.00 });
        m_souvenirs[i].push_back({ "Sticker Pack", 5.00 });
    }

    // Tracking
    m_spentPerCollege.assign(N, 0.0);

    m_saddleIndex = 0;
    m_currentIndex = 0;
    m_totalMiles = 0.0;
    m_grandTotal = 0.0;

    m_visited.assign(N, false);
    m_visited[m_saddleIndex] = true; // saddleback not shown as option
}
