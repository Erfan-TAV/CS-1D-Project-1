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

    // 1. Setup Master List with Search (Student Story)
    campusModel = new QSqlTableModel(this, db);
    campusModel->setTable("campusList");
    campusModel->select();

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(campusModel);
    proxyModel->setFilterKeyColumn(1); // Search by name
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->tableViewSettings->setModel(proxyModel);

    // 2. Setup Starting Point Combo
    comboBoxModel = new QSqlTableModel(this, db);
    comboBoxModel->setTable("campusList");
    comboBoxModel->select();
    ui->comboBox->setModel(comboBoxModel);
    ui->comboBox->setModelColumn(1);
    ui->comboBox->setCurrentIndex(-1);

    // 3. Setup the Trip Results Table Model
    tripModel = new QSqlTableModel(this, db);
    tripModel->setTable("newCampusList");
    tripModel->select();
    // If you have a tableView on page index 1, set its model to tripModel here
}

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
    // 1. Clear previous data
    QSqlQuery clearQuery;
    clearQuery.exec("DELETE FROM newCampusList");

    int startId = -1;
    QVector<int> targets;
    tripPlanner planner;
    TripResult result;

    // 2. Get Starting Point
    if (ui->comboBox->currentIndex() != -1) {
        QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());
        startId = rec.value(0).toInt();
    } else { return; }

    // 3. Collect Selection using Proxy Mapping (Search Accuracy)
    QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();
    for (const QModelIndex &proxyIndex : selectedRows) {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        int id = campusModel->record(sourceIndex.row()).value(0).toInt();
        if (id != startId) targets.append(id);
    }

    // 4. Run Optimization Logic (Recursion)
    result.campusOrder.append(startId);
    if (!targets.isEmpty()) {
        planner.planRecursiveTrip(startId, targets, result);
    }

    // 5. SYNC TABLE TO OPTIMIZED ORDER
    // We re-insert everything into newCampusList in the order stored in result.campusOrder
    for (int i = 0; i < result.campusOrder.size(); ++i) {
        int id = result.campusOrder[i];
        addTripCampus(id, getCampusName(id));
    }
    if (tripModel) tripModel->select(); // Refresh the table view to show optimized order

    // 6. Build UI Timeline (Horizontal Layout)
    clearHorizontalLayout();

    for (int i = 0; i < result.campusOrder.size(); ++i) {
        // Add Campus Label
        QLabel* nameLabel = new QLabel(getCampusName(result.campusOrder[i]));
        nameLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2c3e50;");
        ui->horizontalLayout->addWidget(nameLabel);

        // Add Arrow + Mileage
        if (i < result.campusOrder.size() - 1) {
            double dist = planner.getDistance(result.campusOrder[i], result.campusOrder[i+1]);

            QWidget* spacer = new QWidget();
            QVBoxLayout* vBox = new QVBoxLayout(spacer);
            QLabel* arrow = new QLabel(" ➔ ");
            arrow->setStyleSheet("font-size: 18px; color: #7f8c8d;");
            QLabel* miles = new QLabel(QString::number(dist, 'f', 1) + " mi");
            miles->setStyleSheet("font-size: 10px; color: #16a085;");

            vBox->addWidget(arrow, 0, Qt::AlignCenter);
            vBox->addWidget(miles, 0, Qt::AlignCenter);
            ui->horizontalLayout->addWidget(spacer);
        }
    }

    // 7. Finalize Stats
    ui->totalDistanceLabel->setText("Total Distance: " + QString::number(result.totalDistance, 'f', 2) + " mi");
    ui->totalCampusesLabel->setText("Amount of Campuses: " + QString::number(result.campusOrder.size()));
    ui->tripPlannerStack->setCurrentIndex(1);
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

// void PlanPage::on_startTripButton_clicked() {
//     QSqlQuery clearQuery;
//     clearQuery.exec("DELETE FROM newCampusList");
//
//     // Start Campus
//     if (ui->comboBox->currentIndex() != -1) {
//         QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());
//         // Verify these aren't empty before sending
//         int id = rec.value(0).toInt();
//         QString name = rec.value(1).toString();
//         addTripCampus(id, name);
//     }
//
//     // Selected Campuses
//     QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();
//     for (const QModelIndex &index : selectedRows) {
//         QSqlRecord record = campusModel->record(index.row());
//         addTripCampus(record.value(0).toInt(), record.value(1).toString());
//     }
//
//     if (tripModel) tripModel->select();
//
//     // Switch to the next page
//     if (ui->planOnlyCheckBox->isChecked()) {
//         ui->tripPlannerStack->setCurrentIndex(1);
//     } else {
//         ui->tripPlannerStack->setCurrentIndex(2);
//     }
// }

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

// void PlanPage::setupDatabaseTable() {
//     QSqlDatabase db = QSqlDatabase::database();
//
//     if (!db.isOpen()) {
//         qDebug() << "PlanPage: Database is NOT open at" << db.databaseName();
//         return;
//     }
//
//     // 2. Pass the 'db' object to the model
//     campusModel = new QSqlTableModel(this, db);
//
//     // 3. Match the table name exactly
//     campusModel->setTable("campusList");
//
//     // 4. Important: Set the Edit Strategy before selecting
//     campusModel->setEditStrategy(QSqlTableModel::OnFieldChange);
//
//     // 5. Fetch the data
//     if (!campusModel->select()) {
//         qDebug() << "SQL Error:" << campusModel->lastError().text();
//     } else {
//         qDebug() << "AdminPage: Successfully loaded" << campusModel->rowCount() << "campus rows";
//     }
//
//     // 6. Set the model to your ListView from the UI screenshot
//     ui->tableViewSettings->setModel(campusModel);
//     ui->tableViewSettings->setModelColumn(1); // Column 0 is usually 'campusName'
//
//     // give data to combo box
//     comboBoxModel = new QSqlTableModel(this, db);
//     comboBoxModel->setTable("campusList");
//     comboBoxModel->select();
//     ui->comboBox->setModel(comboBoxModel);
//     ui->comboBox->setModelColumn(1);
//
//     // set placeholder for first option
//     ui->comboBox->setPlaceholderText("--- Select a Campus ---");
//     ui->comboBox->setCurrentIndex(-1); // -1 means "nothing selected"
//
//     // connect combobox changed to update the filter in the table
//     connect(ui->comboBox, &QComboBox::currentTextChanged, this, &PlanPage::updateFilteredTable);
//
//     // update text based on the number of campuses selected
//     // Connect the selection model's signal to a lambda function
//     connect(ui->tableViewSettings->selectionModel(), &QItemSelectionModel::selectionChanged,
//             this, [this](const QItemSelection &selected, const QItemSelection &deselected) {
//
//         // 1. Get the list of all currently selected rows
//         int count = ui->tableViewSettings->selectionModel()->selectedRows().count();
//
//         // 2. Update the TextLabel with the count
//         ui->numCampusRemaingAmount->setText(QString::number(count));
//     });
//
//     // setup model for the current trip table
//     tripModel = new QSqlTableModel(this, db);
//     tripModel->setTable("newCampusList");
//     tripModel->select();
//
// }

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