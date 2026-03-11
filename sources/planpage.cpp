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

#include "databaseHelper.h"


PlanPage::PlanPage(QWidget *parent)
    : DatabasePage(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);

    // set the starting page to the plan setting page
    ui->tripPlannerStack->setCurrentIndex(0);

    setupDatabaseTable();
    setupResultsConnection();
}

PlanPage::~PlanPage()
{
    delete ui;
}

void PlanPage::on_startTripButton_clicked() {
    // 1. Clear the table before starting a new calculation
    clearTripTable();

    int currentOrder = 0;
    int currentCampusID = -1;
    QString currentCampusName = "";

    // 2. Add the Start Campus (The "Origin" - Agile Story 2 & 3)
    if (ui->comboBox->currentIndex() != -1) {
        QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());
        currentCampusID = rec.value("campusID").toInt();
        currentCampusName = rec.value("campusName").toString();

        addTripCampus(currentCampusID, currentCampusName, currentOrder++);
    } else {
        qDebug() << "Please select a starting campus.";
        return; // Abort if no starting campus is selected
    }

    // 3. Retrieve Selected Campuses for the trip
    QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();
    QList<int> unvisitedIDs;
    QMap<int, QString> campusNamesMap;

    // Because we are using QSortFilterProxyModel (Agile Story 4), we must map the index back to the source
    QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(ui->tableViewSettings->model());

    for (const QModelIndex &proxyIndex : selectedRows) {
        QModelIndex sourceIndex = proxy ? proxy->mapToSource(proxyIndex) : proxyIndex;

        // Use campusModel to get the accurate row data
        QSqlRecord record = campusModel->record(sourceIndex.row());
        int id = record.value("campusID").toInt();
        QString name = record.value("campusName").toString();

        // Ensure we don't add the starting campus to the unvisited list twice
        if (id != currentCampusID) {
            unvisitedIDs.append(id);
            campusNamesMap[id] = name;
        }
    }

    // 4. Nearest Neighbor Algorithm (Agile Story 2 & 3)
    // Recursively/Iteratively checks for the closest campus from the current position
    double totalTripDistance = 0.0;

    while (!unvisitedIDs.isEmpty()) {
        int nextCampusID = -1;
        double minDistance = std::numeric_limits<double>::max();

        // Find closest unvisited campus
        for (int id : unvisitedIDs) {
            double dist = getDistanceBetween(currentCampusID, id);
            if (dist < minDistance) {
                minDistance = dist;
                nextCampusID = id;
            }
        }

        if (nextCampusID != -1) {
            totalTripDistance += minDistance;
            currentCampusID = nextCampusID;
            currentCampusName = campusNamesMap[currentCampusID];

            // Log to database
            addTripCampus(currentCampusID, currentCampusName, currentOrder++);

            // Mark as visited
            unvisitedIDs.removeAll(currentCampusID);
        } else {
            qDebug() << "Route disconnected! Ending calculation.";
            break;
        }
    }

    qDebug() << "Total Calculated Distance for Trip:" << totalTripDistance;

    // 5. Refresh the tripModel so the UI updates immediately
    if (tripModel) {
        tripModel->select();
    }

    // 6. Navigation Logic
    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}

void PlanPage::on_resultPlanAnotherButton_clicked() {
    clearTripTable();
    ui->tripPlannerStack->setCurrentIndex(0);
}
void PlanPage::on_planAnotherButton_1_clicked() {
    clearTripTable();
    ui->tripPlannerStack->setCurrentIndex(0);
}
void PlanPage::on_tripPlanStopNextButton_clicked() {
    ui->tripPlannerStack->setCurrentIndex(3);
}

void PlanPage::setupDatabaseTable() {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "PlanPage: Database is NOT open at" << db.databaseName();
        return;
    }

    // 1. Setup Query Model (Agile Story 1: List distances from Saddleback)
    // We use QSqlQueryModel instead of QSqlTableModel so we can execute a custom JOIN query.
    campusModel = new QSqlQueryModel(this);

    // This query pulls all campuses and dynamically calculates their distance to Saddleback.
    QString saddlebackQuery =
        "SELECT c.campusID, c.campusName, "
        "IFNULL((SELECT distance FROM campusDistances cd "
        " JOIN campusList s ON s.campusName LIKE '%Saddleback%' "
        " WHERE (cd.campusID1 = c.campusID AND cd.campusID2 = s.campusID) "
        "    OR (cd.campusID2 = c.campusID AND cd.campusID1 = s.campusID)), 'N/A') AS DistanceToSaddleback "
        "FROM campusList c";

    campusModel->setQuery(saddlebackQuery, db);

    // 2. Setup Proxy Model for Sorting and Filtering (Agile Story 4)
    proxyModel->setSourceModel(campusModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(1); // Default filter checks 'campusName' column

    // 3. Apply to TableView
    ui->tableViewSettings->setModel(proxyModel);
    //ui->tableViewSettings->setSortingEnabled(true); // Agile Story 4: Enables clicking headers to sort

    // give data to combo box (Uses standard table model since we just need names)
    comboBoxModel = new QSqlTableModel(this, db);
    comboBoxModel->setTable("campusList");
    comboBoxModel->select();
    ui->comboBox->setModel(comboBoxModel);
    ui->comboBox->setModelColumn(1);

    // set placeholder for first option
    ui->comboBox->setPlaceholderText("--- Select a Campus ---");
    ui->comboBox->setCurrentIndex(-1); // -1 means "nothing selected"

    // connect combobox changed to update the filter in the table
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &PlanPage::updateFilteredTable);

    // update text based on the number of campuses selected
    connect(ui->tableViewSettings->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection &selected, const QItemSelection &deselected) {
        int count = ui->tableViewSettings->selectionModel()->selectedRows().count();
        ui->numCampusRemaingAmount->setText(QString::number(count));
    });

    // setup model for the current trip table
    tripModel = new QSqlTableModel(this, db);
    tripModel->setTable("tripCampuses");
    tripModel->select();
}

void PlanPage::refreshUI() {
    qDebug() << "PlanPage: Database data re-synced to UI.";
    ui->comboBox->blockSignals(true);

    // reset proxy filters
    proxyModel->setFilterWildcard("");
    comboBoxModel->setFilter("");

    // Reload the lists
    // re-trigger the query for the campus view
    setupDatabaseTable();
    comboBoxModel->select();

    ui->comboBox->setCurrentIndex(-1);
    ui->comboBox->blockSignals(false);
}

void PlanPage::updateFilteredTable(const QString &selectedCampus) {
    if (!proxyModel) return;

    // Agile Story 4: Applying filter through Proxy Model
    // Instead of completely removing the campus, we can filter it out of the view using RegEx.
    // This regex matches any string that is NOT the selected campus.
    QString filterRegex = QString("^(?!%1$).*").arg(QRegularExpression::escape(selectedCampus));

    proxyModel->setFilterRegularExpression(QRegularExpression(filterRegex));

    qDebug() << "Applied Proxy Filter to hide:" << selectedCampus;
}

void PlanPage::setupResultsConnection() {
    QSqlDatabase db = QSqlDatabase::database();

    ui->resultCampusSouvenirPurchases->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tripSouvenirModel = new QSqlTableModel(this, db);
    tripSouvenirModel->setTable("tripSouvenirPurchases");
    tripSouvenirModel->select();

    ui->resultCampusSouvenirPurchases->setModel(tripSouvenirModel);

    ui->resultCampusCombo->setModel(tripModel);
    ui->resultCampusCombo->setModelColumn(3); // Set to campusName index

    connect(ui->resultCampusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlanPage::updateSouvenirFilter);
}

void PlanPage::updateSouvenirFilter(int index) {
    if (index == -1 || !tripSouvenirModel || !tripModel) return;

    QSqlRecord record = tripModel->record(index);
    int selectedID = record.value("campusID").toInt();

    tripSouvenirModel->setFilter(QString("campusID = %1").arg(selectedID));

    if (!tripSouvenirModel->select()) {
        qDebug() << "Souvenir Filter Error:" << tripSouvenirModel->lastError().text();
    }
}