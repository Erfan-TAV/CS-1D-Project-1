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
#include <qsortfilterproxymodel.h>

#include "databaseHelper.h"

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

void PlanPage::setupDatabaseTable() {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) return;

    // 1. STUDENT STORY: Scrollable List with Distances from Saddleback
    // Assuming Saddleback ID is 1. This meets the "Database of college info" requirement.
    saddlebackModel = new QSqlQueryModel(this);
    saddlebackModel->setQuery("SELECT c.campusID, c.campusName, d.distance "
                              "FROM campusList c "
                              "JOIN distances d ON c.campusID = d.destinationID "
                              "WHERE d.startID = 1 ORDER BY d.distance ASC", db);

    // 2. PERFORMANCE: Use a Proxy Model for the searchable list
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(saddlebackModel);
    proxyModel->setFilterKeyColumn(1); // Filter by campusName
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->tableViewSettings->setModel(proxyModel);
    ui->tableViewSettings->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewSettings->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 3. TRIP PLANNER: Setup the Combo Box and Trip Model
    comboBoxModel = new QSqlTableModel(this, db);
    comboBoxModel->setTable("campusList");
    comboBoxModel->select();
    ui->campusComboBox->setModel(comboBoxModel);
    ui->campusComboBox->setModelColumn(1);

    tripModel = new QSqlTableModel(this, db);
    tripModel->setTable("newCampusList");
    tripModel->select();
}

// Fulfills "Check if you can accurately search for a specific college"
void PlanPage::on_searchLineEdit_textChanged(const QString &text) {
    proxyModel->setFilterFixedString(text);
    if (proxyModel->rowCount() == 0 && !text.isEmpty()) {
        ui->numCampusRemaingAmount->setText("Not Found");
    } else {
        int count = ui->tableViewSettings->selectionModel()->selectedRows().count();
        ui->numCampusRemaingAmount->setText(QString::number(count));
    }
}

void PlanPage::on_startTripButton_clicked() {
    int startId = ui->campusComboBox->currentData().toInt();
    QVector<int> targets;
    tripPlanner planner;
    TripResult result;

    // 1. COLLECT SELECTION (Uses Proxy Mapping for search accuracy)
    QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) return;

    for (const QModelIndex &proxyIndex : selectedRows) {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        int id = saddlebackModel->data(saddlebackModel->index(sourceIndex.row(), 0)).toInt();
        if (id != startId) targets.append(id);
    }

    // 2. EXECUTE RECURSIVE LOGIC (Most efficient route)
    result.campusOrder.append(startId);
    if (!targets.isEmpty()) {
        planner.planRecursiveTrip(startId, targets, result);
    }

    // 3. DATABASE SYNC: Update table to be in the same order as the trip
    QSqlQuery clearQuery;
    clearQuery.exec("DELETE FROM newCampusList");
    for (int i = 0; i < result.campusOrder.size(); ++i) {
        addTripCampus(result.campusOrder[i], getCampusName(result.campusOrder[i]));
    }
    tripModel->select();

    // 4. DYNAMIC UI GENERATION (Horizontal Layout with Arrows)
    clearHorizontalLayout();
    for (int i = 0; i < result.campusOrder.size(); ++i) {
        // Campus Container
        QWidget* container = new QWidget();
        QVBoxLayout* vLayout = new QVBoxLayout(container);

        QLabel* nameLabel = new QLabel(getCampusName(result.campusOrder[i]));
        nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
        vLayout->addWidget(nameLabel, 0, Qt::AlignCenter);
        ui->horizontalLayout->addWidget(container);

        if (i < result.campusOrder.size() - 1) {
            double dist = planner.getDistance(result.campusOrder[i], result.campusOrder[i+1]);

            QWidget* transContainer = new QWidget();
            QVBoxLayout* transLayout = new QVBoxLayout(transContainer);

            QLabel* arrow = new QLabel(" ➔ ");
            arrow->setStyleSheet("font-size: 18px; color: #7f8c8d;");
            QLabel* distLabel = new QLabel(QString::number(dist, 'f', 1) + " mi");
            distLabel->setStyleSheet("font-size: 10px; color: #16a085;");

            transLayout->addWidget(arrow, 0, Qt::AlignCenter);
            transLayout->addWidget(distLabel, 0, Qt::AlignCenter);
            ui->horizontalLayout->addWidget(transContainer);
        }
    }

    // 5. UPDATE STATS & NAVIGATE
    ui->totalDistanceLabel->setText("Total Distance: " + QString::number(result.totalDistance, 'f', 2) + " mi");
    ui->totalCampusesLabel->setText("Campuses: " + QString::number(result.campusOrder.size()));

    ui->tripPlannerStack->setCurrentIndex(ui->planOnlyCheckBox->isChecked() ? 1 : 2);
}

void PlanPage::clearHorizontalLayout() {
    if (!ui->horizontalLayout) return;
    QLayoutItem *item;
    while ((item = ui->horizontalLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->hide();
            delete item->widget();
        }
        delete item;
    }
}

void PlanPage::addTripCampus(int id, QString name) {
    QSqlQuery query;
    query.prepare("INSERT INTO newCampusList (campusID, campusName) VALUES (:id, :name)");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.exec();
}

void PlanPage::on_planAnotherButton_clicked() {
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

void PlanPage::refreshUI() {
    qDebug() << "PlanPage: Database data re-synced to UI.";

    ui->comboBox->blockSignals(true);

    // revert any table filters
    campusModel->setFilter("");
    comboBoxModel->setFilter("");

    // 1. Reload the main campus list
    campusModel->select();
    comboBoxModel->select();

    ui->comboBox->setCurrentIndex(-1);

    ui->comboBox->blockSignals(false);

    // 2. Figure out which campus was selected before the refresh
    QModelIndex currentIndex = ui->tableViewSettings->currentIndex();
    if (currentIndex.isValid()) {
        QSqlRecord record = campusModel->record(currentIndex.row());
    }
}

void PlanPage::updateFilteredTable(const QString &selectedCampus) {
    if (!campusModel) return;

    // 1. Apply the filter
    QString filterStr = QString("campusName != '%1'").arg(selectedCampus);
    campusModel->setFilter(filterStr);

    // 2. Execute
    if (!campusModel->select()) {
        qDebug() << "SQL Error:" << campusModel->lastError().text();
    }

    // 3. Print the "Truth" to the console
    qDebug() << "--- Filter Debug ---";
    qDebug() << "Applied Filter:" << campusModel->filter();
    qDebug() << "Rows Found:" << campusModel->rowCount();

    // If rowCount is 0, the filter is the problem.
    // If it's more than 0 but the table is empty, the View/UI is the problem.
}