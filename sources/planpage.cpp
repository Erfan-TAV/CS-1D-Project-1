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

void PlanPage::refreshUI() {
    qDebug() << "PlanPage: Database data re-synced to UI.";
    // 1. Reload the main campus list
    campusModel->select();

    // 2. Figure out which campus was selected before the refresh
    QModelIndex currentIndex = ui->tableViewSettings->currentIndex();
    if (currentIndex.isValid()) {
        QSqlRecord record = campusModel->record(currentIndex.row());
        int campusId = record.value("campusId").toInt();

        // 3. Re-apply the filter to the souvenirs so they stay visible
        // souvenirModel->setFilter(QString("campusId = %1").arg(campusId));
        // souvenirModel->select();
        // }
    }
}