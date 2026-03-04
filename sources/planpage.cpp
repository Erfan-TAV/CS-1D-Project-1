#include "planpage.h"
#include "ui_planpage.h"
#include "TripPlanner.h"
#include <QSqlQuery>
#include <QDebug>

PlanPage::PlanPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);
    ui->tripPlannerStack->setCurrentIndex(0);
    ui->campusTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

PlanPage::~PlanPage()
{
    delete ui;
}

void PlanPage::on_startTripButton_clicked()
{
    // --- START AGILE LOGIC IMPLEMENTATION ---
    
    // 1. Get IDs from the 'Selected' table (tableView_2)
    QVector<int> selectedIds;
    QAbstractItemModel* model = ui->tableView_2->model();
    if (model) {
        for(int i = 0; i < model->rowCount(); ++i) {
            // Assumes ID is stored in Column 0
            selectedIds.append(model->index(i, 0).data().toInt());
        }
    }

    // 2. Get Starting Campus (Assuming a comboBox in your UI)
    int startId = ui->campusComboBox->currentData().toInt();

    // 3. Initialize and execute recursive planner
    TripPlanner planner;
    TripResult result;
    result.campusOrder.append(startId);
    planner.planRecursiveTrip(startId, selectedIds, result);

    // 4. Update the result UI (InfoPage)
    // We emit a signal to MainWindow to handle the cross-page data transfer
    emit tripCalculationFinished(result);

    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}

void PlanPage::on_planAnotherButton_clicked()
{
    ui->tripPlannerStack->setCurrentIndex(0);
    // Clear selection tables logic would go here
}

void PlanPage::on_planAnotherButton_1_clicked()
{
    ui->tripPlannerStack->setCurrentIndex(0);
}

void PlanPage::on_tripPlanStopNextButton_clicked()
{
    // Logic for individual stops
    ui->tripPlannerStack->setCurrentIndex(3);
}
