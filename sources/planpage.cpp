#include "planpage.h"
#include "ui_planpage.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <qsqlerror.h>
#include <QSqlRecord>
#include <QTimer>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QDebug>

#include "databaseHelper.h"

PlanPage::PlanPage(QWidget *parent)
    : DatabasePage(parent)
    , ui(new Ui::PlanPage)
{
    ui->setupUi(this);
    qDebug() << "[PLANPAGE] Constructor started.";

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
    // 1. Clear the table before starting a new calculation
    qDebug() << "[UI] Start Trip Button clicked.";
    clearTripTable();

    int currentCampusID = -1;
    QString currentCampusName = "";

    // 2. Add the Start Campus (The "Origin")
    // FIX: We must extract the data from the record before assigning it to the variables
    if (ui->comboBox->currentIndex() != -1) {
        // We use the comboBoxModel directly to get the record
        QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());

        // Assigning the actual database values to our tracking variables
        currentCampusID = rec.value("campusID").toInt();
        currentCampusName = rec.value("campusName").toString();

        qDebug() << "[UI] Starting Campus selected:" << currentCampusName << "(ID:" << currentCampusID << ")";

        // Add the origin stop as visitOrder 0
        addTripCampus(currentCampusID, currentCampusName, 0);
    } else {
        qDebug() << "[UI] WARNING: No starting campus selected.";
        return;
    }

    // 3. Retrieve Selected Campuses from the TableView
    QModelIndexList selectedRows = ui->tableViewSettings->selectionModel()->selectedRows();
    QList<int> unvisitedIDs;

    qDebug() << "[UI] Rows selected in list:" << selectedRows.size();

    for (const QModelIndex &proxyIndex : selectedRows) {
        // Map the proxy (sorted/filtered) index back to the actual source model
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        QSqlRecord record = campusModel->record(sourceIndex.row());
        int id = record.value("campusID").toInt();

        // Only add to the unvisited list if it's not the starting campus
        if (id != currentCampusID) {
            unvisitedIDs.append(id);
        }
    }

    // 4. Trigger the decoupled recursive algorithm
    qDebug() << "[ALGO] --- Starting RECURSIVE Trip Calculation ---";

    // Start the recursion:
    // currentCampusID: the origin
    // unvisitedIDs: the list of schools to hit
    // 0.0: initial total distance
    // 1: the visit order for the first stop after the origin
    calculateEfficientTrip(currentCampusID, unvisitedIDs, 0.0, 1);

    populateTripSouvenirs();
    setupTripStopModel();

    currentTripOrder = 0;
    int campusID = getCampusIDByOrder(currentTripOrder);

    showCurrentCampusSouvenirs(campusID);
    updateStopLabels();

    // 5. Update UI state
    if (tripModel) {
        tripModel->select();
    }

    // 6. Navigation Logic
    if (ui->planOnlyCheckBox->isChecked()) {
        qDebug() << "[UI] Navigating to Plan-Only view.";
        ui->tripPlannerStack->setCurrentIndex(1); // Index for the scrollable trip layout
        renderTrip(); // Draw the widgets based on the newly calculated data
    } else {
        qDebug() << "[UI] Navigating to Full-Trip view.";
        ui->tripPlannerStack->setCurrentIndex(2); // Index for the standard results page
    }
}

// ==========================================
// UI & DATABASE SETUP (Requirement 2 & 3)
// ==========================================
void PlanPage::setupDatabaseTable() {
    qDebug() << "[DEBUG] Entering setupDatabaseTable";

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "[DEBUG] Database not open - exiting setup";
        return;
    }

    if (!ui) return;

    // 1. Initialize ALL models
    if (!campusModel)   campusModel = new QSqlQueryModel(this);
    if (!proxyModel)    proxyModel = new QSortFilterProxyModel(this);
    if (!comboBoxModel) comboBoxModel = new QSqlTableModel(this, db);
    if (!tripModel)     tripModel = new QSqlTableModel(this, db);

    // 2. Setup Source Data
    campusModel->setQuery("SELECT campusID, campusName FROM campusList", db);

    // 3. Setup Proxy Model
    proxyModel->setSourceModel(campusModel);
    // Explicitly tell the proxy to filter based on Column 1 (Campus Name)
    proxyModel->setFilterKeyColumn(1);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // 4. Attach to TableView
    if (ui->tableViewSettings) {
        ui->tableViewSettings->setModel(proxyModel);
        ui->tableViewSettings->setModelColumn(1);

        if (ui->tableViewSettings->selectionModel()) {
            connect(ui->tableViewSettings->selectionModel(), &QItemSelectionModel::selectionChanged,
                    this, &PlanPage::updateSelectionCount, Qt::UniqueConnection);
        }
    }

    // 5. Setup ComboBox
    comboBoxModel->setTable("campusList");
    comboBoxModel->select();

    if (ui->comboBox) {
        ui->comboBox->blockSignals(true);

        ui->comboBox->setModel(comboBoxModel);
        ui->comboBox->setModelColumn(1);

        // We set index to -1 AFTER the model is attached and data is loaded
        ui->comboBox->setCurrentIndex(-1);
        ui->comboBox->setPlaceholderText("Select Starting Campus...");

        ui->comboBox->blockSignals(false);

        // Disconnect old connections to prevent double-firing, then reconnect
        disconnect(ui->comboBox, &QComboBox::currentTextChanged, this, &PlanPage::updateFilteredTable);
        connect(ui->comboBox, &QComboBox::currentTextChanged,
                this, &PlanPage::updateFilteredTable, Qt::UniqueConnection);
    }

    tripModel->setTable("tripCampuses");
    tripModel->select();

    qDebug() << "[DEBUG] Exiting setupDatabaseTable normally.";
}

void PlanPage::refreshUI() {
    qDebug() << "[UI] Refreshing UI...";

    if (ui && ui->comboBox) {
        ui->comboBox->blockSignals(true);

        // 1. Clear the Proxy Filter first
        if (proxyModel) {
            proxyModel->setFilterFixedString("");
        }

        // 2. Refresh Database Queries
        if (campusModel) {
            campusModel->setQuery("SELECT campusID, campusName FROM campusList");
        }

        if (comboBoxModel) {
            comboBoxModel->select();
        }

        if (tripModel) {
            tripModel->select();
        }

        // 3. Reset UI state
        ui->comboBox->setCurrentIndex(-1);
        ui->comboBox->blockSignals(false);

        qDebug() << "[UI] Refresh complete. ComboBox reset and filters cleared.";
    }
}

void PlanPage::updateFilteredTable(const QString &selectedCampus) {
    if (!proxyModel) return;

    if (selectedCampus.isEmpty()) {
        qDebug() << "[UI] Filter cleared.";
        proxyModel->setFilterFixedString("");
        return;
    }

    qDebug() << "[UI] Filtering out selected campus:" << selectedCampus;

    // Ensure the proxy is looking at the correct column (Column 1 = Campus Name)
    proxyModel->setFilterKeyColumn(1);

    // Regex Explanation:
    // ^ = Start of string
    // (?! ... $) = Negative lookahead: "Do not match if the string following is exactly this"
    // .* = Match everything else
    // \\Q and \\E = Treat the campus name as literal text (escapes special chars)
    QString pattern = QString("^(?!\\Q%1\\E$).*").arg(selectedCampus);

    QRegularExpression re(pattern);
    proxyModel->setFilterRegularExpression(re);

    // Force the view to update immediately
    if (ui->tableViewSettings) {
        ui->tableViewSettings->viewport()->update();
    }
}

void PlanPage::setupResultsConnection() {
    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << "[UI] Setting up results page connections.";

    ui->resultCampusSouvenirPurchases->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    tripSouvenirModel = new QSqlTableModel(this, db);
    tripSouvenirModel->setTable("tripSouvenirPurchases");
    tripSouvenirModel->setFilter("quantity > 0");
    tripSouvenirModel->select();

    ui->resultCampusSouvenirPurchases->setModel(tripSouvenirModel);

    ui->resultCampusSouvenirPurchases->hideColumn(0);
    ui->resultCampusSouvenirPurchases->hideColumn(1);
    ui->resultCampusSouvenirPurchases->verticalHeader()->setVisible(false);

    // Combo box setup
    ui->resultCampusCombo->setModel(tripModel);
    ui->resultCampusCombo->setModelColumn(3);

    connect(ui->resultCampusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlanPage::updateSouvenirFilter);

    // 🔹 Force first campus selection
    if (tripModel->rowCount() > 0) {
        ui->resultCampusCombo->setCurrentIndex(0);
        updateSouvenirFilter(0);
    }
}

void PlanPage::updateSouvenirFilter(int index) {
    if (index == -1 || !tripSouvenirModel || !tripModel) return;

    QSqlRecord record = tripModel->record(index);
    int selectedID = record.value("campusID").toInt();

    tripSouvenirModel->setFilter(
        QString("campusID = %1 AND quantity > 0").arg(selectedID)
    );

    tripSouvenirModel->select();
    ui->resultCampusSouvenirPurchases->viewport()->update();
}

void PlanPage::on_resultPlanAnotherButton_clicked()
{
    qDebug() << "[UI] Plan Another clicked.";
    clearTripTable();
    ui->tripPlannerStack->setCurrentIndex(0);
}
void PlanPage::on_planOnlyPlanAnotherButton_clicked()
{
    qDebug() << "[UI] Plan Another clicked.";
    clearTripTable();
    ui->tripPlannerStack->setCurrentIndex(0);
}
void PlanPage::on_tripPlanStopNextButton_clicked()
{
    currentTripOrder++;

    int campusID = getCampusIDByOrder(currentTripOrder);

    if (campusID == -1) {
        ui->tripPlannerStack->setCurrentIndex(3);
        updateSouvenirFilter(ui->resultCampusCombo->currentIndex());
        return;
    }

    showCurrentCampusSouvenirs(campusID);
    updateStopLabels();   // <-- AFTER order change
}

// This function creates one "Campus -> Distance" block
QWidget* PlanPage::createStopWidget(QString name, int distance, bool showArrow) {
    QWidget *stop = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(stop);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(0);

    // --- 1. The Campus Card ---
    QFrame *card = new QFrame();
    card->setStyleSheet(
        "QFrame {"
        "  background-color: palette(button);"
        "  border: 1px solid palette(mid);"
        "  border-radius: 8px;"
        "  padding: 10px;"
        "}"
        );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("font-weight: bold; color: palette(window-text); font-size: 18px; border: none;");
    nameLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(nameLabel);

    mainLayout->addWidget(card);

    // --- 2. The Connector (Only if showArrow is true) ---
    if (showArrow) {
        QWidget *connector = new QWidget();
        QVBoxLayout *vbox = new QVBoxLayout(connector);
        vbox->setAlignment(Qt::AlignCenter);
        vbox->setContentsMargins(0, 0, 0, 0);
        vbox->setSpacing(2);

        QLabel *arrow = new QLabel("──────▶");
        arrow->setStyleSheet("color: palette(window-text); font-weight: bold; font-size: 16px;");
        arrow->setAlignment(Qt::AlignCenter);
        vbox->addWidget(arrow);

        // Only show distance if it's a positive value (greater than 0)
        if (distance > 0) {
            QLabel *distLabel = new QLabel(QString::number(distance) + " miles");
            distLabel->setStyleSheet("font-size: 12px; color: palette(placeholder-text);");
            distLabel->setAlignment(Qt::AlignCenter);
            vbox->addWidget(distLabel);
        } else {
            // Spacer to keep arrow vertically aligned with others
            QLabel *spacer = new QLabel(" ");
            spacer->setStyleSheet("font-size: 10px;");
            vbox->addWidget(spacer);
        }
        mainLayout->addWidget(connector);
    }

    return stop;
}

void PlanPage::renderTrip() {
    // 1. Get the layout from the scroll area
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!layout) {
        qDebug() << "[ERROR] Could not find layout for scrollAreaWidgetContents";
        return;
    }

    // 2. Clear previous UI elements to prevent "ghosting" from old trips
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 3. Fetch the trip sequence from the temporary trip table
    // We order by visitOrder to ensure the tour displays in the correct path sequence
    QSqlQuery query("SELECT campusID, campusName, visitOrder FROM tripCampuses ORDER BY visitOrder ASC");

    while (query.next()) {
        int currentID = query.value("campusID").toInt();
        QString name = query.value("campusName").toString();
        int currentOrder = query.value("visitOrder").toInt();

        int distanceToNext = 0;

        // 4. Find the NEXT campus in the sequence to get the distance
        QSqlQuery nextQuery;
        nextQuery.prepare("SELECT campusID FROM tripCampuses WHERE visitOrder = :nextOrder");
        nextQuery.bindValue(":nextOrder", currentOrder + 1);

        if (nextQuery.exec() && nextQuery.next()) {
            int nextID = nextQuery.value(0).toInt();

            // REFACTORED: Use the helper function from databaseHelper.h
            // This handles the bi-directional SQL check (ID1->ID2 or ID2->ID1)
            double distResult = getDistanceBetween(currentID, nextID);

            // Check for the "Not Found" sentinel value (999999.0)
            if (distResult < 999998.0) {
                distanceToNext = static_cast<int>(distResult);
            } else {
                qDebug() << "[WARNING] No distance found between ID" << currentID << "and" << nextID;
            }
        }

        // 5. Add the campus stop widget
        // The 'true' flag ensures an arrow and distance label are drawn after this stop
        layout->addWidget(createStopWidget(name, distanceToNext, true));
    }

    // 6. Add the final "Goal" flag
    // We pass 'false' for showArrow because no path follows the final destination
    layout->addWidget(createStopWidget("🏁 Trip Finished", 0, false));

    // 7. Add stretch at the end to keep the trip sequence left-aligned
    layout->addStretch();

    qDebug() << "[UI] Trip rendering complete.";
}

void PlanPage::updateSelectionCount() {
    if (!ui->tableViewSettings->selectionModel()) return;

    int count = ui->tableViewSettings->selectionModel()->selectedRows().count();
    ui->numCampusRemaingAmount->setText(QString::number(count));
    qDebug() << "[UI] Selection changed. Current count:" << count;
}

void PlanPage::setupTripStopModel()
{
    QSqlDatabase db = QSqlDatabase::database();

    if (!tripStopSouvenirModel)
        tripStopSouvenirModel = new QSqlTableModel(this, db);

    tripStopSouvenirModel->setTable("tripSouvenirPurchases");
    tripStopSouvenirModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    ui->stopSouvenirTableView->setModel(tripStopSouvenirModel);

    ui->stopSouvenirTableView->verticalHeader()->setVisible(false); // hide row numbers
    ui->stopSouvenirTableView->hideColumn(0);
    ui->stopSouvenirTableView->hideColumn(1);

    ui->stopSouvenirTableView->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    ui->stopSouvenirTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    tripStopSouvenirModel->select();
}

void PlanPage::showCurrentCampusSouvenirs(int campusID)
{
    if (!tripStopSouvenirModel) return;

    QString filter = QString("campusID = %1").arg(campusID);
    tripStopSouvenirModel->setFilter(filter);
    tripStopSouvenirModel->select();
}

int PlanPage::getCampusIDByOrder(int order)
{
    QSqlQuery query;
    query.prepare("SELECT campusID FROM tripCampuses WHERE visitOrder = :order");
    query.bindValue(":order", order);

    if (query.exec() && query.next())
        return query.value(0).toInt();

    return -1;
}

void PlanPage::updateStopLabels()
{
    int currentID = getCampusIDByOrder(currentTripOrder);
    int nextID = getCampusIDByOrder(currentTripOrder + 1);

    ui->stopCurrentCampusLabel->setText(getCampusName(currentID));

    if (nextID != -1) {
        ui->stopNextCampusLabel->setText(getCampusName(nextID));
        ui->stopNextCampusDIstanceLabel->setText(
            QString::number(getDistanceBetween(currentID, nextID)) + " miles"
        );

        // Normal case
        ui->tripPlanStopNextButton->setText("Next Campus");
    }
    else {
        ui->stopNextCampusLabel->setText("Trip Complete");
        ui->stopNextCampusDIstanceLabel->setText("");

        // Last stop
        ui->tripPlanStopNextButton->setText("Finish Trip");
    }
}