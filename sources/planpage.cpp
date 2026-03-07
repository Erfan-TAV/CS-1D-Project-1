#include "planpage.h"

#include <QSqlError>
#include "ui_planpage.h"
#include "../headers/tripPlanner.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include "adminpage.h"
#include "ui_adminpage.h"
#include <qsqlerror.h>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QTimer>
#include "databaseHelper.h"
#include <QFileDialog>
#include <QStandardPaths>



PlanPage::PlanPage(QWidget *parent)
    : DatabasePage(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);

    // set the starting page to the plan setting page
    ui->tripPlannerStack->setCurrentIndex(0);

    // ------------------------------------------------------------------------------------
    // setup the table in tripPlan
    // Set the first column (Campus Name) to stretch and fill the table
    // ui->campusTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    // Hide row headers
    // ui->tableWidget->verticalHeader()->setVisible(false);
    // ------------------------------------------------------------------------------------
    // setup the table in tripPlan
}

PlanPage::~PlanPage()
{
    delete ui;
}

void PlanPage::on_startTripButton_clicked()
{
    // ui->tabWidget->setCurrentIndex(0);

    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}

// void PlanPage::on_startTripButton_clicked()
// {
//     int startId = 0;
//     QVector<int> targets;
//     tripPlanner planner;
//     TripResult result;
//
//     // 1. SAFE CHECK: Ensure the table and its model exist
//     if (ui->tableViewSettings != nullptr && ui->tableViewSettings->model() != nullptr) {
//
//         QAbstractItemModel* model = ui->tableViewSettings->model();
//         int rowCount = model->rowCount();
//
//         // 2. STORY LOGIC: If user selected campuses, use them.
//         // If the table is empty, we handle it gracefully.
//         if (rowCount > 0) {
//             // Get starting campus from combo box
//             startId = ui->campusComboBox->currentData().toInt();
//
//             // Loop through all selected campuses in the table
//             for(int i = 0; i < rowCount; ++i) {
//                 int id = model->index(i, 0).data(Qt::UserRole).toInt();
//
//                 // Don't add the starting campus to the "targets" list to avoid loops
//                 if (id != startId) {
//                     targets.append(id);
//                 }
//             }
//         } else {
//             qDebug() << "No campuses selected in tableViewSettings.";
//             return; // Exit early if there's nothing to plan
//         }
//     } else {
//         qDebug() << "Critical Error: tableViewSettings model is null (Code 139 prevention).";
//         return;
//     }
//
//     // 3. EXECUTE RECURSIVE LOGIC
//     // tripPlanner planner;
//     // TripResult result;
//     result.campusOrder.append(startId);
//
//     if (!targets.isEmpty()) {
//         planner.planRecursiveTrip(startId, targets, result);
//     }
//
//     // 4. UPDATE DISPLAY
//     // If you aren't using signals, you must call the display function directly
//     // Assuming 'infoPage' is accessible here:
//     // ui->infoPageWidget->displayTripResults(result);
//
//     // Original UI switching logic
//     if (ui->planOnlyCheckBox->isChecked()) {
//         ui->tripPlannerStack->setCurrentIndex(1);
//     } else {
//         ui->tripPlannerStack->setCurrentIndex(2);
//     }
// }

void PlanPage::on_planAnotherButton_clicked() { ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_planAnotherButton_1_clicked() { ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_tripPlanStopNextButton_clicked() { ui->tripPlannerStack->setCurrentIndex(3); }

void PlanPage::setupDatabaseTable() {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "PlanPage: Database is NOT open at" << db.databaseName();
        return;
    }
}

// void PlanPage::displayRoute(const TripResult &result) {
//     // 1. Clear previous route from the layout to avoid ghosting
//     QLayoutItem *item;
//     while ((item = ui->routeLayout->takeAt(0)) != nullptr) {
//         if (item->widget()) delete item->widget();
//         delete item;
//     }
//
//     // 2. Iterate through the recursive results
//     for (int i = 0; i < result.campusOrder.size(); ++i) {
//         // Create a container for the individual campus
//         QWidget* campusContainer = new QWidget();
//         QVBoxLayout* vLayout = new QVBoxLayout(campusContainer);
//
//         // Fetch Campus Name (Assuming you have a helper function)
//         QString name = getCampusNameById(result.campusOrder[i]);
//
//         // Add Campus Name Label
//         QLabel* nameLabel = new QLabel(name);
//         nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
//         vLayout->addWidget(nameLabel, 0, Qt::AlignCenter);
//
//         ui->routeLayout->addWidget(campusContainer);
//
//         // 3. Add the Arrow and Distance if there is a next stop
//         if (i < result.campusOrder.size() - 1) {
//             // Container for arrow and distance
//             QWidget* transitionContainer = new QWidget();
//             QVBoxLayout* transLayout = new QVBoxLayout(transitionContainer);
//
//             // Distance Label (shown below/middle of the arrow)
//             double segmentDist = getSegmentDistance(result.campusOrder[i], result.campusOrder[i+1]);
//             QLabel* distLabel = new QLabel(QString::number(segmentDist) + " mi");
//             distLabel->setStyleSheet("color: gray; font-size: 10px;");
//
//             QLabel* arrowLabel = new QLabel(" ➔ ");
//             arrowLabel->setStyleSheet("font-size: 18px;");
//
//             transLayout->addWidget(arrowLabel, 0, Qt::AlignCenter);
//             transLayout->addWidget(distLabel, 0, Qt::AlignCenter);
//
//             ui->routeLayout->addWidget(transitionContainer);
//         }
//     }
//
//     // 4. Update the Stats (Bottom Right)
//     ui->totalDistanceLabel->setText("Total Distance: " + QString::number(result.totalDistance, 'f', 2) + " mi");
//     ui->totalCampusesLabel->setText("Amount of Campuses: " + QString::number(result.campusOrder.size()));
// }
