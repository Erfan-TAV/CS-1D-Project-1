#include "planpage.h"
#include "ui_planpage.h"
#include "TripPlanner.h" // Include the new header

void PlanPage::on_startTripButton_clicked()
{
    // 1. Get the starting campus from the UI (assuming a combo box)
    int startCampusId = ui->startCampusCombo->currentData().toInt();

    // 2. Get the list of campuses the user wants to visit
    // This example assumes you have a list of checked IDs from your UI
    QVector<int> selectedCampuses = getSelectedCampusIds(); 

    // 3. Setup the TripResult container
    TripResult myTrip;
    myTrip.campusOrder.append(startCampusId); // Start the list with the beginning
    myTrip.totalDistance = 0.0;

    // 4. Run the recursive planner
    TripPlanner planner;
    planner.planShortestTrip(startCampusId, selectedCampuses, myTrip);

    // 5. Update the UI Display (Requirement: Check if display is accurate)
    displayTripResults(myTrip);
    
    // Switch stack to the results page
    ui->tripPlannerStack->setCurrentIndex(2);
}

void PlanPage::displayTripResults(const TripResult& result) {
    ui->campusTable->setRowCount(0);
    
    for (int i = 0; i < result.campusOrder.size(); ++i) {
        int id = result.campusOrder[i];
        int row = ui->campusTable->rowCount();
        ui->campusTable->insertRow(row);
        
        // Fetch name from DB to display
        QSqlQuery nameQuery;
        nameQuery.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
        nameQuery.bindValue(":id", id);
        nameQuery.exec();
        nameQuery.next();
        
        ui->campusTable->setItem(row, 0, new QTableWidgetItem(nameQuery.value(0).toString()));
    }
    
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(result.totalDistance));
}
