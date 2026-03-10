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

         // 2. Add the Start Campus (The "Origin")
  if (ui->comboBox->currentIndex() != -1) {
    // We use the comboBoxModel directly to get the record
    QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());

    int id = rec.value("campusID").toInt();
    QString name = rec.value("campusName").toString();

    // Add as the first stop (Order 0)
    addTripCampus(id, name, currentOrder++);
  }

         // 3. Add Selected Campuses (The "Destinations")
  QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();

  for (const QModelIndex &index : selectedRows) {
    // Important: Get the record from the campusModel at the specific row
    QSqlRecord record = campusModel->record(index.row());

    int id = record.value("campusID").toInt();
    QString name = record.value("campusName").toString();

    // Add and increment the order
    addTripCampus(id, name, currentOrder++);
  }

         // 4. Refresh the tripModel so the UI (like resultCampusCombo) updates immediately
  if (tripModel) {
    tripModel->select();
  }

         // 5. Navigation Logic
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

    // give data to combo box
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
    // Connect the selection model's signal to a lambda function
    connect(ui->tableViewSettings->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection &selected, const QItemSelection &deselected) {

        // 1. Get the list of all currently selected rows
        int count = ui->tableViewSettings->selectionModel()->selectedRows().count();

        // 2. Update the TextLabel with the count
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

void PlanPage::setupResultsConnection() {
  QSqlDatabase db = QSqlDatabase::database();

  ui->resultCampusSouvenirPurchases->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

         // 1. Initialize the Souvenir Model
  tripSouvenirModel = new QSqlTableModel(this, db);
  tripSouvenirModel->setTable("tripSouvenirPurchases");
  tripSouvenirModel->select();

         // 2. Set the model to your TableView
  ui->resultCampusSouvenirPurchases->setModel(tripSouvenirModel);

         // 3. Setup the Campus Combo Box
  ui->resultCampusCombo->setModel(tripModel);
  ui->resultCampusCombo->setModelColumn(3); // Set to campusName index

         // 4. Connect the signal to filter the souvenirs when a campus is picked
  connect(ui->resultCampusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &PlanPage::updateSouvenirFilter);
}

void PlanPage::updateSouvenirFilter(int index) {
  if (index == -1 || !tripSouvenirModel || !tripModel) return;

         // 1. Get the campusID from the tripModel record at the selected index
  QSqlRecord record = tripModel->record(index);
  int selectedID = record.value("campusID").toInt();

         // 2. Apply the filter to the souvenir model
         // This is the SQL WHERE clause: "campusID = 5"
  tripSouvenirModel->setFilter(QString("campusID = %1").arg(selectedID));

         // 3. Execute the filtered query
  if (!tripSouvenirModel->select()) {
    qDebug() << "Souvenir Filter Error:" << tripSouvenirModel->lastError().text();
  }
}
