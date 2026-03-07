#include "planpage.h"
#include "ui_planpage.h"
#include "../headers/tripPlanner.h"
#include <QSqlTableModel>
#include "adminpage.h"
#include "ui_adminpage.h"
#include <qsqlerror.h>
#include <QSqlRecord>
#include <QTimer>
#include <QFileDialog>

PlanPage::PlanPage(QWidget *parent)
    : DatabasePage(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);

    // set the starting page to the plan setting page
    ui->tripPlannerStack->setCurrentIndex(0);

    setupDatabaseTable();
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

    // 2. Pass the 'db' object to the model
    campusModel = new QSqlTableModel(this, db);

    // 3. Match the table name exactly
    campusModel->setTable("campusList");

    // 4. Important: Set the Edit Strategy before selecting
    campusModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    // 5. Fetch the data
    if (!campusModel->select()) {
        qDebug() << "SQL Error:" << campusModel->lastError().text();
    } else {
        qDebug() << "AdminPage: Successfully loaded" << campusModel->rowCount() << "campus rows";
    }

    // 6. Set the model to your ListView from the UI screenshot
    ui->tableViewSettings->setModel(campusModel);
    ui->tableViewSettings->setModelColumn(1); // Column 0 is usually 'campusName'
}

void PlanPage::refreshUI() {
    qDebug() << "PlanPage: Database data re-synced to UI.";
    // 1. Reload the main campus list
    campusModel->select();

    // 2. Figure out which campus was selected before the refresh
    QModelIndex currentIndex = ui->tableViewSettings->currentIndex();
    if (currentIndex.isValid()) {
        QSqlRecord record = campusModel->record(currentIndex.row());
    }
}