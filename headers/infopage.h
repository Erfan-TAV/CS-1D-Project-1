/**
 * @file infopage.h
 * @brief Header file for the InfoPage class, responsible for displaying trip summaries.
 */
#ifndef INFOPAGE_H
#define INFOPAGE_H

#include <QWidget>

#include "databasePage.h"
#include "tripPlanner.h"

namespace Ui {
    class InfoPage;
}
/**
 * @class InfoPage
 * @brief A user interface page that displays detailed information and results for a planned trip.
 * * This class inherits from @ref DatabasePage and specializes in visualizing 
 * the data processed by the TripPlanner, such as total distance traveled 
 * and souvenir costs.
 * * 
 */
class InfoPage : public DatabasePage
{
    Q_OBJECT

public:
/**
     * @brief Constructs an InfoPage widget.
     * @param parent The parent QWidget, defaulting to nullptr.
     */
    explicit InfoPage(QWidget *parent = nullptr);
    /**
     * @brief Destructor to clean up UI resources.
     */
    ~InfoPage();
    /**
     * @brief Fills  UI elements with results from a completed trip calculation.
     * @param result A @ref TripResult struct containing distances, visited campuses, and costs.
     */
    void displayTripResults(const TripResult &result);

private:
    Ui::InfoPage *ui; /**< Pointer to the Qt Designer generated UI class. */

private slots:
/**
     * @brief Refreshes the display by pulling the latest relevant data from the database.
     * * This overrides @ref DatabasePage::refreshUI and ensures that any changes to 
     * campus names or souvenir prices are reflected on the info screen.
     */
    void refreshUI() override;
};

#endif // INFOPAGE_H
