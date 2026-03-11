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
    // 1. Clear the table before starting a new calculation
    qDebug() << "[UI] Start Trip Button clicked.";
    clearTripTable();

    int currentCampusID = -1;
    QString currentCampusName = "";

    // 2. Add the Start Campus (The "Origin")
    if (ui->comboBox->currentIndex() != -1) {
        // We use the comboBoxModel directly to get the record
        QSqlRecord rec = comboBoxModel->record(ui->comboBox->currentIndex());

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
        renderTrip();
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
    ui->tripPlannerStack->setCurrentIndex(3);
}

// This function creates one "Campus -> Distance" block
QWidget* PlanPage::createCampusWidget(QString name, QString distance, bool isLast) {
    QWidget *item = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(item);

    // 1. The Campus Name
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(nameLabel);

    if (!isLast) {
        // 2. The Arrow and Distance (Stacked vertically)
        QWidget *arrowContainer = new QWidget();
        QVBoxLayout *vbox = new QVBoxLayout(arrowContainer);
        vbox->setSpacing(0);

        QLabel *arrowLabel = new QLabel("------->");
        QLabel *distLabel = new QLabel(distance + "m");
        distLabel->setAlignment(Qt::AlignCenter);
        distLabel->setStyleSheet("color: #666; font-size: 10px;");

        vbox->addWidget(arrowLabel);
        vbox->addWidget(distLabel);
        layout->addWidget(arrowContainer);
    }

    return item;
}

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
    nameLabel->setStyleSheet("font-weight: bold; color: palette(window-text); font-size: 13px; border: none;");
    nameLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(nameLabel);

    mainLayout->addWidget(card);

    // --- 2. The Connector (Only if showArrow is true) ---
    if (showArrow) {
        QWidget *connector = new QWidget();
        QVBoxLayout *vbox = new QVBoxLayout(connector);
        vbox->setAlignment(Qt::AlignCenter);
        vbox->setContentsMargins(15, 0, 15, 0);
        vbox->setSpacing(2);

        QLabel *arrow = new QLabel("────────▶");
        arrow->setStyleSheet("color: palette(window-text); font-weight: bold; font-size: 16px;");
        arrow->setAlignment(Qt::AlignCenter);
        vbox->addWidget(arrow);

        // Only show distance if it's a positive value (greater than 0)
        if (distance > 0) {
            QLabel *distLabel = new QLabel(QString::number(distance) + "m");
            distLabel->setStyleSheet("font-size: 10px; color: palette(placeholder-text);");
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
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(ui->scrollAreaWidgetContents->layout());

    // 1. Clear previous UI elements
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 2. Fetch the trip sequence
    // We order by visitOrder to ensure the tour is in the correct sequence
    QSqlQuery query("SELECT campusID, campusName, visitOrder FROM tripCampuses ORDER BY visitOrder ASC");

    while (query.next()) {
        int currentID = query.value("campusID").toInt();
        QString name = query.value("campusName").toString();
        int currentOrder = query.value("visitOrder").toInt();

        int distance = 0;

        // 3. Logic to find the nextID and Distance
        QSqlQuery nextQuery;
        nextQuery.prepare("SELECT campusID FROM tripCampuses WHERE visitOrder = :nextOrder");
        nextQuery.bindValue(":nextOrder", currentOrder + 1);

        if (nextQuery.exec() && nextQuery.next()) {
            int nextID = nextQuery.value(0).toInt();

            // Fetch distance using your existing table structure
            // We check both directions (ID1->ID2 and ID2->ID1)
            QSqlQuery distQuery;
            distQuery.prepare("SELECT distance FROM campusDistances "
                              "WHERE (campusID1 = :curr AND campusID2 = :next) "
                              "OR (campusID1 = :next AND campusID2 = :curr)");
            distQuery.bindValue(":curr", currentID);
            distQuery.bindValue(":next", nextID);

            if (distQuery.exec() && distQuery.next()) {
                distance = distQuery.value(0).toInt();
            }
        }

        // Add the campus stop. Every campus in this loop gets an arrow.
        layout->addWidget(createStopWidget(name, distance, true));
    }

    // 4. Final Flag (No arrow follows this)
    layout->addWidget(createStopWidget("🏁 Trip Finished", 0, false));

    // Add stretch to keep everything left-aligned
    layout->addStretch();
}
