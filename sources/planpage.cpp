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
    : QWidget(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);
    ui->tripPlannerStack->setCurrentIndex(0);

    setupDatabaseTable();
    // ui->tableViewSettings->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

PlanPage::~PlanPage() { delete ui; }

void PlanPage::on_startTripButton_clicked()
{
    int startId = 0;
    QVector<int> targets;
    tripPlanner planner;
    TripResult result;

    // 1. SAFE CHECK: Ensure the table and its model exist
    if (ui->tableViewSettings != nullptr && ui->tableViewSettings->model() != nullptr) {

        QAbstractItemModel* model = ui->tableViewSettings->model();
        int rowCount = model->rowCount();

        // 2. STORY LOGIC: If user selected campuses, use them.
        // If the table is empty, we handle it gracefully.
        if (rowCount > 0) {
            // Get starting campus from combo box
            startId = ui->campusComboBox->currentData().toInt();

            // Loop through all selected campuses in the table
            for(int i = 0; i < rowCount; ++i) {
                int id = model->index(i, 0).data(Qt::UserRole).toInt();

                // Don't add the starting campus to the "targets" list to avoid loops
                if (id != startId) {
                    targets.append(id);
                }
            }
        } else {
            qDebug() << "No campuses selected in tableViewSettings.";
            return; // Exit early if there's nothing to plan
        }
    } else {
        qDebug() << "Critical Error: tableViewSettings model is null (Code 139 prevention).";
        return;
    }

    // 3. EXECUTE RECURSIVE LOGIC
    // tripPlanner planner;
    // TripResult result;
    result.campusOrder.append(startId);

    if (!targets.isEmpty()) {
        planner.planRecursiveTrip(startId, targets, result);
    }

    // 4. UPDATE DISPLAY
    // If you aren't using signals, you must call the display function directly
    // Assuming 'infoPage' is accessible here:
    // ui->infoPageWidget->displayTripResults(result);

    // Original UI switching logic
    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}


void PlanPage::on_planAnotherButton_clicked() { ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_planAnotherButton_1_clicked() { ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_tripPlanStopNextButton_clicked() { ui->tripPlannerStack->setCurrentIndex(3); }

void PlanPage::setupDatabaseTable() {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "PlanPage: Database is NOT open at" << db.databaseName();
        return;
    }

    campusModel = new QSqlTableModel(this, db);

    // Link the table's model to the proper table in the database
    campusModel->setTable("campusList");

    // TODO: change to no editing
    campusModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    // Fetch the data
    if (!campusModel->select()) {
        qDebug() << "SQL Error:" << campusModel->lastError().text();
    } else {
        qDebug() << "PlanPage: Successfully loaded" << campusModel->rowCount() << "campus rows";
    }

    // link the model to the ui
    ui->tableViewSettings->setModel(campusModel);
    ui->tableViewSettings->setModelColumn(1);
}