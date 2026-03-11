/**
 * @file planpage.h
 * @brief Header file for the trip planning and execution interface.
 * @author Shahob Shahmirzadi
 */

#ifndef PLANPAGE_H
#define PLANPAGE_H

#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QRegularExpression>
#include "databasePage.h"

namespace Ui {
    class PlanPage;
}

/**
 * @class PlanPage
 * @brief Manages the multi-step trip planning process and the live tour interface.
 * @details This class handles three distinct UI states via a QStackedWidget:
 * 1. **Settings (Index 0):** Users select a starting campus and destination schools.
 * 2. **Plan-Only (Index 1):** A visual horizontal scroll view of the calculated path.
 * 3. **Full-Trip (Index 2):** An interactive "Stop-by-Stop" shopping interface.
 * 4. **Results (Index 3):** A summary of purchases and total distances for the trip.
 */
class PlanPage : public DatabasePage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for PlanPage.
     * @param parent Pointer to the parent widget.
     */
    explicit PlanPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor for PlanPage.
     */
    ~PlanPage() override;

private:
    /**
     * @brief Initializes database models and connects UI elements for the setup screen.
     * @details Sets up models for the campus selection list, the starting campus 
     * combo box, and the temporary trip table.
     */
    void setupDatabaseTable();

    /**
     * @brief Configures the final results page UI and purchase models.
     */
    void setupResultsConnection();

    /**
     * @brief Dynamically renders a horizontal visual representation of the trip.
     * @details Clears the scroll area and populates it with 'Stop' widgets 
     * containing campus names and mileages.
     */
    void renderTrip();

    /**
     * @brief Factory function to create a UI component for a single trip leg.
     * @param name The name of the campus.
     * @param distance The mileage to the next stop.
     * @param showArrow If true, draws a connector arrow after the card.
     * @return A pointer to the constructed stop widget.
     */
    QWidget* createStopWidget(QString name, int distance, bool showArrow);

    Ui::PlanPage *ui;                               /**< Pointer to the UI setup. */
    QSqlQueryModel* campusModel = nullptr;         /**< Model for the campus selection list. */
    QSqlTableModel* comboBoxModel = nullptr;       /**< Model for the starting campus selector. */
    QSqlTableModel* tripModel = nullptr;           /**< Model for the current calculated trip sequence. */
    QSqlTableModel* tripSouvenirModel = nullptr;   /**< Model for viewing purchased items in results. */
    QSqlTableModel *tripStopSouvenirModel = nullptr; /**< Model for live shopping at a specific stop. */
    QSortFilterProxyModel* proxyModel;             /**< Filters out the starting campus from the selection list. */
    int currentTripOrder = 0;                      /**< Tracks the current stop index during a live tour. */

private slots:
    /**
     * @brief Initiates the recursive trip algorithm based on user selections.
     */
    void on_startTripButton_clicked();

    /**
     * @brief Finalizes the trip and returns to the planning screen.
     */
    void on_resultPlanAnotherButton_clicked();

    /**
     * @brief Returns to the planning screen from the plan-only view.
     */
    void on_planOnlyPlanAnotherButton_clicked();

    /**
     * @brief Advances the user to the next campus in the tour sequence.
     */
    void on_tripPlanStopNextButton_clicked();

    /**
     * @brief Filters the selection table to hide the campus selected as the starting point.
     * @param selectedCampus The name of the campus to exclude from the list.
     */
    void updateFilteredTable(const QString &selectedCampus);

    /**
     * @brief Filters the souvenir list to show only items for the selected campus.
     * @param index The index of the campus in the result model.
     */
    void updateSouvenirFilter(int index);

    /**
     * @brief Updates the UI label showing how many campuses are currently selected.
     */
    void updateSelectionCount();

    /**
     * @brief Initializes the table view used for buying souvenirs during the trip.
     */
    void setupTripStopModel();

    /**
     * @brief Updates the shopping table to show souvenirs for a specific ID.
     * @param campusID The unique ID of the campus the user is "visiting."
     */
    void showCurrentCampusSouvenirs(int campusID);

    /**
     * @brief Helper to find a campus ID based on its position in the trip.
     * @param order The visit order (0, 1, 2...).
     * @return The campusID, or -1 if not found.
     */
    int getCampusIDByOrder(int order);

    /**
     * @brief Updates labels (Current/Next) during the live tour phase.
     */
    void updateStopLabels();

    /**
     * @brief Sums the distance of the calculated trip and saves it to the database.
     */
    void updateTotalDistance();

    /**
     * @brief Calculates how much money was spent at a specific stop.
     * @param campusID The ID of the campus to calculate.
     */
    void updateCampusSpent(int campusID);

    /**
     * @brief Calculates the total monetary cost of the entire trip.
     */
    void updateTotalTripPurchased();

    /**
     * @brief Refreshes all models to ensure data reflects the latest database state.
     * @note Overrides DatabasePage::refreshUI().
     */
    void refreshUI() override;

signals:
    /**
     * @brief Signal emitted when a new total distance is calculated.
     * @param distance The total mileage of the trip.
     */
    void distanceCalculated(const int distance);
};

#endif // PLANPAGE_H
