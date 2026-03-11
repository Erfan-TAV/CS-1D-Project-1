#include "planpage.h"
#include "ui_planpage.h"
#include "../headers/tripPlanner.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include "adminpage.h"
#include "ui_adminpage.h"
#include <qsqlerror.h>
#include <QSqlRecord>
#include <QTimer>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <limits>
#include <QSqlQuery>
#include <QDebug>

#include "databaseHelper.h"

PlanPage::PlanPage(QWidget *parent)
    : DatabasePage(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);
    ui->tripPlannerStack->setCurrentIndex(0);

    // Initialize proxyModel to prevent null pointer crashes
    proxyModel = new QSortFilterProxyModel(this);

    setupDatabaseTable();
    setupResultsConnection();
}

PlanPage::~PlanPage()
{
    delete ui;
}

void PlanPage::on_startTripButton_clicked() {
    clearTripTable();

    int currentCampusID = -1;
    QString currentCampusName = "";

    // 1. Handle Start Campus
    if (ui->comboBox->currentIndex() != -1) {
        QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());
        currentCampusID = rec.value("campusID").toInt();
        currentCampusName = rec.value("campusName").toString();

        addTripCampus(currentCampusID, currentCampusName, 0);
    } else {
        qDebug() << "Please select a starting campus.";
        return;
    }

    // 2. Retrieve Selected Campuses
    QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();
    QList<int> unvisitedIDs;

    for (const QModelIndex &proxyIndex : selectedRows) {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        QSqlRecord record = campusModel->record(sourceIndex.row());
        int id = record.value("campusID").toInt();

        if (id != currentCampusID) {
            unvisitedIDs.append(id);
        }
    }

    // 3. Trigger the decoupled algorithm
    calculateEfficientTrip(currentCampusID, unvisitedIDs);

    // 4. Update UI state
    if (tripModel) {
        tripModel->select();
    }

    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}

// ==========================================
// UI & DATABASE SETUP (Requirement 2 & 3)
// ==========================================
void PlanPage::setupDatabaseTable() {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "PlanPage: Database is NOT open at" << db.databaseName();
        return;
    }

    // 1. Universal Query (Requirement 3: Removed Saddleback hardcode)
    campusModel = new QSqlQueryModel(this);
    QString universalQuery = "SELECT campusID, campusName FROM campusList";
    campusModel->setQuery(universalQuery, db);

    // 2. Setup Proxy Model
    proxyModel->setSourceModel(campusModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(1); // Filter checks 'campusName' column

    // 3. Apply to TableView and format to look like a List (Requirement 2)
    ui->tableViewSettings->setModel(proxyModel);

    // Hide the row numbers (vertical header)
    ui->tableViewSettings->verticalHeader()->setVisible(false);

    // Hide the ID column, leaving only the Name column visible
    ui->tableViewSettings->hideColumn(0);

    // Stretch the name column to fill the empty space
    ui->tableViewSettings->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // Enable clicking the column header to sort alphabetically
    ui->tableViewSettings->setSortingEnabled(true);

    // 4. Setup ComboBox
    comboBoxModel = new QSqlTableModel(this, db);
    comboBoxModel->setTable("campusList");
    comboBoxModel->select();
    ui->comboBox->setModel(comboBoxModel);
    ui->comboBox->setModelColumn(1);
    ui->comboBox->setPlaceholderText("--- Select a Campus ---");
    ui->comboBox->setCurrentIndex(-1);

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &PlanPage::updateFilteredTable);

    connect(ui->tableViewSettings->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this]() {
        int count = ui->tableViewSettings->selectionModel()->selectedRows().count();
        ui->numCampusRemaingAmount->setText(QString::number(count));
    });

    tripModel = new QSqlTableModel(this, db);
    tripModel->setTable("tripCampuses");
    tripModel->select();
}

void PlanPage::refreshUI() {
    ui->comboBox->blockSignals(true);
    proxyModel->setFilterWildcard("");
    comboBoxModel->setFilter("");
    setupDatabaseTable();
    comboBoxModel->select();
    ui->comboBox->setCurrentIndex(-1);
    ui->comboBox->blockSignals(false);
}

void PlanPage::updateFilteredTable(const QString &selectedCampus) {
    if (!proxyModel) return;
    QString filterRegex = QString("^(?!%1$).*").arg(QRegularExpression::escape(selectedCampus));
    proxyModel->setFilterRegularExpression(QRegularExpression(filterRegex));
}

void PlanPage::setupResultsConnection() {
    QSqlDatabase db = QSqlDatabase::database();

    ui->resultCampusSouvenirPurchases->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tripSouvenirModel = new QSqlTableModel(this, db);
    tripSouvenirModel->setTable("tripSouvenirPurchases");
    tripSouvenirModel->select();

    ui->resultCampusSouvenirPurchases->setModel(tripSouvenirModel);

    ui->resultCampusCombo->setModel(tripModel);
    ui->resultCampusCombo->setModelColumn(3);

    connect(ui->resultCampusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlanPage::updateSouvenirFilter);
}

void PlanPage::updateSouvenirFilter(int index) {
    if (index == -1 || !tripSouvenirModel || !tripModel) return;

    QSqlRecord record = tripModel->record(index);
    int selectedID = record.value("campusID").toInt();

    tripSouvenirModel->setFilter(QString("campusID = %1").arg(selectedID));
    tripSouvenirModel->select();
}

void PlanPage::on_resultPlanAnotherButton_clicked() { clearTripTable(); ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_planAnotherButton_1_clicked() { clearTripTable(); ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_tripPlanStopNextButton_clicked() { ui->tripPlannerStack->setCurrentIndex(3); }