#include "planpage.h"
#include "ui_planpage.h"
#include "../headers/tripPlanner.h"
#include <QSqlQuery>

PlanPage::PlanPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);
    ui->tripPlannerStack->setCurrentIndex(0);
    ui->campusTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

PlanPage::~PlanPage() { delete ui; }

void PlanPage::on_startTripButton_clicked()
{
    int startId;
    QVector<int> targets;
    tripPlanner planner;
    TripResult result;

    // CHECK FOR STORY 2: "Initial 11 from Saddleback"
    // TODO: fix the logic below, give error 139
    // If no custom selections are made in tableView, default to the Initial 11 logic
    // if (ui->tableView->model()->rowCount() == 0) {
    //     startId = 1; // Assuming 1 is Saddleback ID
    //     QSqlQuery query("SELECT campusID FROM campusList LIMIT 11");
    //     while(query.next()) {
    //         int id = query.value(0).toInt();
    //         if (id != startId) targets.append(id);
    //     }
    // }
    // // CHECK FOR STORY 1: "Student Custom Trip"
    // else {
    //     startId = ui->comboBox->currentData().toInt();
    //     QAbstractItemModel* model = ui->tableView->model();
    //     for(int i = 0; i < model->rowCount(); ++i) {
    //         targets.append(model->index(i, 0).data(Qt::UserRole).toInt());
    //     }
    // }

    // RUN THE SHARED RECURSIVE LOGIC
    result.campusOrder.append(startId);
    planner.planRecursiveTrip(startId, targets, result);

    // Pass data to InfoPage (Assuming a signal is connected in MainWindow)
    // emit tripCalculationFinished(result);

    // Original Page Switching Logic
    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1); // Stop Page
    } else {
        ui->tripPlannerStack->setCurrentIndex(2); // Result Page
    }
}

void PlanPage::on_planAnotherButton_clicked() { ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_planAnotherButton_1_clicked() { ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_tripPlanStopNextButton_clicked() { ui->tripPlannerStack->setCurrentIndex(3); }
