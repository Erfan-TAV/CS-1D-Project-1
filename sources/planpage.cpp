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
    qDebug() << "[PLANPAGE] Constructor started.";
    ui->setupUi(this);

    // INITIALIZE THESE FIRST
    campusModel = new QSqlQueryModel(this);
    proxyModel = new QSortFilterProxyModel(this);

    ui->tripPlannerStack->setCurrentIndex(0);

    setupDatabaseTable();
    setupResultsConnection();
    qDebug() << "[PLANPAGE] Constructor finished.";
}

PlanPage::~PlanPage()
{
    delete ui;
}



void PlanPage::on_startTripButton_clicked() {
    qDebug() << "[UI] Start Trip Button clicked.";
    clearTripTable();

    int currentCampusID = -1;
    QString currentCampusName = "";

    // 1. Handle Start Campus
    if (ui->comboBox->currentIndex() != -1) {
        QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());
        currentCampusID = rec.value("campusID").toInt();
        currentCampusName = rec.value("campusName").toString();

        qDebug() << "[UI] Starting Campus selected:" << currentCampusName << "(ID:" << currentCampusID << ")";
        addTripCampus(currentCampusID, currentCampusName, 0);
    } else {
        qDebug() << "[UI] WARNING: No starting campus selected.";
        return;
    }

    // 2. Retrieve Selected Campuses
    QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();
    QList<int> unvisitedIDs;

    qDebug() << "[UI] Rows selected in list:" << selectedRows.size();

    for (const QModelIndex &proxyIndex : selectedRows) {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        QSqlRecord record = campusModel->record(sourceIndex.row());
        int id = record.value("campusID").toInt();

        if (id != currentCampusID) {
            unvisitedIDs.append(id);
        }
    }

    // 3. Trigger the decoupled algorithm
    qDebug() << "[ALGO] --- Starting RECURSIVE Trip Calculation ---";

    // Start the recursion: 0.0 is initial distance, 1 is the first stop after origin
    calculateEfficientTrip(currentCampusID, unvisitedIDs, 0.0, 1);

    // 4. Update UI state
    if (tripModel) {
        tripModel->select();
    }

    if (ui->planOnlyCheckBox->isChecked()) {
        qDebug() << "[UI] Navigating to Plan-Only view.";
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        qDebug() << "[UI] Navigating to Full-Trip view.";
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}

// ==========================================
// UI & DATABASE SETUP (Requirement 2 & 3)
// ==========================================
void PlanPage::setupDatabaseTable() {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "[DB] CRITICAL: Database is NOT open!";
        return;
    }

    // 1. Universal Query
    campusModel = new QSqlQueryModel(this);
    QString universalQuery = "SELECT campusID, campusName FROM campusList";
    campusModel->setQuery(universalQuery, db);
    qDebug() << "[DB] Universal Campus Query executed. Row count:" << campusModel->rowCount();

    // 2. Setup Proxy Model
    proxyModel->setSourceModel(campusModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(1);

    // 3. Apply to ListView (tableViewSettings)
    ui->tableViewSettings->setModel(proxyModel);
    ui->tableViewSettings->setModelColumn(1); // Show Names only

    // Enable sort programmatically
    proxyModel->sort(1, Qt::AscendingOrder);
    qDebug() << "[UI] ListView model attached and sorted alphabetically.";

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
        qDebug() << "[UI] Selection changed. Current count:" << count;
    });

    tripModel = new QSqlTableModel(this, db);
    tripModel->setTable("tripCampuses");
    tripModel->select();
}

void PlanPage::refreshUI() {
    qDebug() << "[UI] Refreshing UI...";
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
    qDebug() << "[UI] Start Campus changed. Filtering out:" << selectedCampus;
    QString filterRegex = QString("^(?!%1$).*").arg(QRegularExpression::escape(selectedCampus));
    proxyModel->setFilterRegularExpression(QRegularExpression(filterRegex));
}

void PlanPage::setupResultsConnection() {
    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << "[UI] Setting up results page connections.";

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

    qDebug() << "[UI] Viewing souvenirs for ID:" << selectedID;
    tripSouvenirModel->setFilter(QString("campusID = %1").arg(selectedID));
    tripSouvenirModel->select();
}

void PlanPage::on_resultPlanAnotherButton_clicked() { qDebug() << "[UI] Plan Another clicked."; clearTripTable(); ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_planAnotherButton_1_clicked() { qDebug() << "[UI] Plan Another clicked."; clearTripTable(); ui->tripPlannerStack->setCurrentIndex(0); }
void PlanPage::on_tripPlanStopNextButton_clicked() { ui->tripPlannerStack->setCurrentIndex(3); }
