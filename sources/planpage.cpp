#include "planpage.h"
#include "ui_planpage.h"
#include "TripPlanner.h"

void PlanPage::on_startTripButton_clicked()
{
    // Agile Requirement: Start at Saddleback (Assuming ID 1 is Saddleback)
    int saddlebackId = 1; 

    // Agile Requirement: Initial 11 campuses
    // We fetch the first 11 IDs from the database
    QVector<int> initial11Ids;
    QSqlQuery query("SELECT campusID FROM campusList LIMIT 11");
    while(query.next()) {
        int id = query.value(0).toInt();
        if (id != saddlebackId) initial11Ids.append(id);
    }

    // Initialize Trip
    TripPlanner planner;
    TripResult result;
    result.campusOrder.append(saddlebackId);

    // Agile Requirement: Recursive checker for closest campus
    planner.planRecursiveTrip(saddlebackId, initial11Ids, result);

    // Emit result so InfoPage can display it
    emit tripCalculationFinished(result);

    // Original UI switching logic
    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}
