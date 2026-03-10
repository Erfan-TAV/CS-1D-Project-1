/**
 * @file planpage.h
 * @brief Header file for the PlanPage class, handling trip configuration and selection.
 */
#ifndef PLANPAGE_H
#define PLANPAGE_H

#include <QSqlTableModel>
#include <QWidget>
#include "databasePage.h"

namespace Ui {
    class PlanPage;
}
/**
 * @class PlanPage
 * @brief This is used for setting up and initiating a college tour trip.
 * * This class allows users to select a starting campus, filter potential destinations, 
 * and manage the current list of campuses to be visited. It uses multiple SQL models 
 * to provide real-time views of the database.
 * * 
 */
class PlanPage : public DatabasePage
{
    Q_OBJECT

public:
/**
     * @brief Constructs a PlanPage and initializes the UI components.
     * @param parent The parent QWidget, defaulting to nullptr.
     */
    explicit PlanPage(QWidget *parent = nullptr);
    /**
     * @brief Destructor to safely clean up the UI and allocated models.
     */
    ~PlanPage() override;

private:
    Ui::PlanPage *ui; /**< Pointer to the Qt Designer generated UI class. */
    /**
     * @brief Configures the QSqlTableModels and binds them to the UI views.
     * Sets up the headers, edit strategies, and table names for campus selection and the trip list.
     */
    void setupDatabaseTable();
    QSqlTableModel* campusModel;     /**< Model for the campus selection list on settings page. */
    QSqlTableModel* comboBoxModel;   /**< Model for the combobox on settings page. */
    QSqlTableModel* tripModel;       /**< Model for the current trip table */

private slots:
/**
     * @brief Slot triggered when the user initiates the trip calculation.
     * Hands off the selected campuses to the @ref tripPlanner.
     */
    void on_startTripButton_clicked();
    /**
     * @brief Navigates the UI back to the initial planning state to start a new tour.
     */
    void on_planAnotherButton_clicked();
    /**
     * @brief Secondary reset handler for the planning wizard navigation.
     */
    void on_planAnotherButton_1_clicked();
    /**
     * @brief Moves the user to the next step in the trip planning sequence.
     */
    void on_tripPlanStopNextButton_clicked();
    /**
     * @brief Filters the destination table based on the selected starting campus.
     * Ensures that the starting campus is not listed as a potential destination.
     * @param selectedCampus The name of the campus to exclude from the filter.
     */
    void updateFilteredTable(const QString &selectedCampus);

    /**
     * @brief Refreshes all internal SQL models to reflect current database state.
     * Overrides @ref DatabasePage::refreshUI to ensure selection lists are up-to-date.
     */
    void refreshUI() override;

signals:
/* Note: No signals currently defined for this page. */
};

#endif // PLANPAGE_H
