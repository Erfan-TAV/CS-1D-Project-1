/**
 * @file infopage.h
 * @brief Handles the display of final trip statistics, including costs and distances.
 * @author Erfan Tavassoli
 */

#ifndef INFOPAGE_H
#define INFOPAGE_H

#include <QWidget>
#include <QStandardItemModel>
#include "databasePage.h"

// Forward declaration for trip results
struct TripResult;

namespace Ui {
    class InfoPage;
}

/**
 * @class InfoPage
 * @brief Provides a summary interface displaying trip results and purchase history.
 * @details This class is responsible for presenting the final itinerary, total items 
 * purchased, total cost spent, and the cumulative distance traveled. It pulls data 
 * from the 'tripInfo' and 'tripInfoDistances' tables to populate its view.
 */
class InfoPage : public DatabasePage {
    Q_OBJECT

public:
    /**
     * @brief Constructor for InfoPage.
     * @param parent Pointer to the parent widget.
     */
    explicit InfoPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor for InfoPage.
     */
    ~InfoPage() override;

    /**
     * @brief Formats and displays the results of the pathfinding algorithm.
     * @details Updates the UI labels with the total distance and populates 
     * the table with the sequence of campuses visited.
     * @param result A TripResult struct containing the ordered IDs and total miles.
     */
    void displayTripResults(const TripResult &result);

    /**
     * @brief Re-syncs the summary table and labels with the current database state.
     * @details Fetches souvenir purchase records (name, quantity, price) from the 
     * database and calculates total spent and total items. It also triggers 
     * a distance recalculation.
     * @note Overrides DatabasePage::refreshUI().
     */
    void refreshUI() override;

    /**
     * @brief Clears all displayed data and resets counters to zero.
     * @details Used when starting a new plan or clearing an existing itinerary.
     */
    void resetUI();

private:
    /**
     * @brief Calculates the cumulative distance of the current trip from the database.
     * @details Queries the 'tripInfoDistances' table to sum up all legs of the journey 
     * and updates the totalDistanceAmount label.
     */
    void calculateTotalDistance();

    Ui::InfoPage *ui;                 /**< Pointer to the UI setup. */
    QStandardItemModel *model;       /**< Model used to manage the summary table data. */
};

#endif // INFOPAGE_H
