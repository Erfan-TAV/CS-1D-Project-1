#include "adminpage.h"
#include "ui_adminpage.h"
#include <qsqlerror.h>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QTimer>
#include "databaseHelper.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QTableWidget>

AdminPage::AdminPage(QWidget* parent) :
    DatabasePage(parent), ui(new Ui::AdminPage)
{
    ui->setupUi(this);

    // set the page to login screen by default
    ui->adminPageStack->setCurrentIndex(0);

    setupLoginPage();

    setupDatabaseTable();
}

AdminPage::~AdminPage()
{
    delete ui;
}

// TODO: add proper login logic
void AdminPage::handleLogin() {
    if (ui->usernameField->text() == "admin" && ui->passwordField->text() == "admin") {
        // 1. Switch the internal stack (Dashboard is index 1)
        ui->adminPageStack->setCurrentIndex(1);

        // 2. Tell the MainWindow to show the success message
        emit notifyStatus("Login Successful");

        ui->usernameField->clear();
        ui->passwordField->clear();
    } else {
        emit notifyStatus("Login Failed");
    }
}

void AdminPage::setupDatabaseTable() {
    // 1. Get the connection you opened in main.cpp
    // If DbManager didn't give it a specific name, it's the default:
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "AdminPage: Database is NOT open at" << db.databaseName();
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
    ui->campusList->setModel(campusModel);
    ui->campusList->setModelColumn(1); // Column 0 is usually 'campusName'


    // setup the souvenir table
    // 1. Initialize the Souvenir Model (the right-hand table)
    souvenirModel = new QSqlTableModel(this, QSqlDatabase::database());
    souvenirModel->setTable("souvenirs"); // Replace with your actual table name
    souvenirModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    ui->tableView->setModel(souvenirModel);
    // hide the campus id
    ui->tableView->setColumnHidden(0, true);
    // add nice headers for the columns
    souvenirModel->setHeaderData(1, Qt::Horizontal, "Souvenir Name");
    souvenirModel->setHeaderData(2, Qt::Horizontal, "Price ($)");

    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    // 2. Handle the "Click" on the Campus List
    connect(ui->campusList, &QListView::clicked, this, [this](const QModelIndex &index) {
        // Get the full record for the row you just clicked
        QSqlRecord record = campusModel->record(index.row());

        // Update the LineEdit with the name
        QString name = record.value("campusName").toString(); // Use your actual column name
        ui->collegeNameLineEdit->setText(name);

        // Filter the Souvenirs table by Campus ID
        int campusId = record.value("campusId").toInt(); // Use your actual ID column name
        souvenirModel->setFilter(QString("campusId = %1").arg(campusId));
        souvenirModel->select();
    });

    // 3. Handle live-editing the name back to the DB
    connect(ui->collegeNameLineEdit, &QLineEdit::textEdited, this, [this](const QString &newText) {
        QModelIndex currentIndex = ui->campusList->currentIndex();
        if (currentIndex.isValid()) {
            // This updates the model, which updates the DB because of OnFieldChange
            campusModel->setData(campusModel->index(currentIndex.row(), 1), newText);
        }
    });

    // When the user presses Enter in the LineEdit
    connect(ui->collegeNameLineEdit, &QLineEdit::returnPressed, this, [this]() {
        QModelIndex currentIndex = ui->campusList->currentIndex();
        if (currentIndex.isValid()) {
            // Create an index specifically for Column 1 (campusName)
            QModelIndex nameIndex = campusModel->index(currentIndex.row(), 1);

            QString newName = ui->collegeNameLineEdit->text();

            // Update the name, not the ID!
            if (campusModel->setData(nameIndex, newName)) {
                campusModel->submitAll(); // Push to project1.db
                emit notifyStatus("Campus updated successfully!");
            } else {
                qDebug() << "Update failed:" << campusModel->lastError().text();
            }
        }
    });

    // enable +/- for souvenirs
    connect(ui->removeSouvenirButton, &QPushButton::clicked, this, [this]() {
        QModelIndex currentIndex = ui->tableView->currentIndex();
        if (!currentIndex.isValid()) return;

        // 1. Get the identifying data from the selected row
        // Column 1 is souvenirName (based on your DB screenshot)
        QString nameToDelete = souvenirModel->data(souvenirModel->index(currentIndex.row(), 1)).toString();
        int campusID = souvenirModel->data(souvenirModel->index(currentIndex.row(), 0)).toInt();

        // 2. Run a direct SQL Query to bypass the model's cache
        QSqlQuery query;
        query.prepare("DELETE FROM souvenirs WHERE souvenirName = :name AND campusID = :id");
        query.bindValue(":name", nameToDelete);
        query.bindValue(":id", campusID);

        if (query.exec()) {
            // 3. The data is GONE from the .db file. Now force the UI to match.
            // Re-applying the filter and calling select() flushes the ghost rows.
            souvenirModel->setFilter(QString("campusID = %1").arg(campusID));
            souvenirModel->select();

            qDebug() << "Direct SQL Delete successful for:" << nameToDelete;
        } else {
            qDebug() << "SQL Error:" << query.lastError().text();
        }
    });
    connect(ui->addSouvenirButton, &QPushButton::clicked, this, [this]() {
        // 1. Get the Campus ID from the left list to link the new souvenir
        QModelIndex campusIndex = ui->campusList->currentIndex();
        if (!campusIndex.isValid()) return;

        // Assuming Column 0 of campusModel is campusID
        int currentCampusID = campusModel->data(campusModel->index(campusIndex.row(), 0)).toInt();

        // 2. Insert a new row at the end of the filtered list
        int rowCount = souvenirModel->rowCount();
        souvenirModel->insertRow(rowCount);

        // 3. Set the Foreign Key (campusID) automatically in Column 0 (hidden)
        souvenirModel->setData(souvenirModel->index(rowCount, 0), currentCampusID);

        // 4. Select the new row and start editing the Name (Column 1)
        QModelIndex nameIndex = souvenirModel->index(rowCount, 1);
        ui->tableView->setCurrentIndex(nameIndex);
        ui->tableView->edit(nameIndex);
    });
    // this jumps the user to the price column when done writing the price
    connect(souvenirModel, &QSqlTableModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        // We only care if the 'Souvenir Name' (Column 1) was the one changed
        if (topLeft.column() == 1) {

            // 1. Identify the 'Price' cell in the same row (Column 2)
            QModelIndex priceIndex = souvenirModel->index(topLeft.row(), 2);

            // 2. Set the focus to the table and start editing the price
            ui->tableView->setCurrentIndex(priceIndex);

            // We use a timer with 0ms to let the current 'Enter' event finish
            // before opening the next editor, preventing a focus glitch.
            QTimer::singleShot(0, this, [this, priceIndex]() {
                ui->tableView->edit(priceIndex);
            });
        }
    });

    // enable +/- for campusList
    connect(ui->removeCampusButton, &QPushButton::clicked, this, [this]() {
        QModelIndex currentIndex = ui->campusList->currentIndex();
        if (!currentIndex.isValid()) return;

        int idToDelete = campusModel->data(campusModel->index(currentIndex.row(), 0)).toInt();
        QSqlDatabase db = QSqlDatabase::database();

        // Start a transaction to ensure all deletes happen together or none at all
        db.transaction();

        QSqlQuery query;

             // 1. Delete associated souvenirs
        query.prepare("DELETE FROM souvenirs WHERE campusID = :id");
        query.bindValue(":id", idToDelete);
        if (!query.exec()) {
            qDebug() << "Souvenir delete failed:" << query.lastError().text();
            db.rollback();
            return;
        }

        // 2. Delete associated distances (where  is campusID1)
        // This will remove all rows where this campus is the 'source' of a distance
        query.prepare("DELETE FROM campusDistances WHERE campusID1 = :id OR campusID2 = :id");
        query.bindValue(":id", idToDelete);
        if (!query.exec()) {
            qDebug() << "Distance delete failed:" << query.lastError().text();
            db.rollback();
            return;
        }

        // 3. Delete the actual campus
        query.prepare("DELETE FROM campusList WHERE campusID = :id");
        query.bindValue(":id", idToDelete);
        if (!query.exec()) {
            qDebug() << "Campus delete failed:" << query.lastError().text();
            db.rollback();
            return;
        }

        // Commit the changes to disk
        if (db.commit()) {
            campusModel->select(); // Refresh UI list
            ui->collegeNameLineEdit->clear();
            souvenirModel->setFilter("campusID = -1");
            souvenirModel->select();
            qDebug() << "Campus and all related data successfully deleted.";
        }
    });
    connect(ui->addCampusButton, &QPushButton::clicked, this, [this]() {
        // 1. Determine a unique placeholder name
        int count = 1;
        QSqlQuery countQuery;
        // Count how many campuses currently start with "New College"
        countQuery.prepare("SELECT COUNT(*) FROM campusList WHERE campusName LIKE 'New College%'");
        if (countQuery.exec() && countQuery.next()) {
            count = countQuery.value(0).toInt() + 1;
        }

        QString uniquePlaceholder = QString("New College %1").arg(count);
        int rowCount = campusModel->rowCount();

        if (campusModel->insertRow(rowCount)) {
            // 2. Set the unique name
            campusModel->setData(campusModel->index(rowCount, 1), uniquePlaceholder);

            if (campusModel->submitAll()) {
                ui->campusList->setCurrentIndex(campusModel->index(rowCount, 1));
                ui->collegeNameLineEdit->setText(uniquePlaceholder);
                ui->collegeNameLineEdit->setFocus();
                ui->collegeNameLineEdit->selectAll();

                // 3. Trigger the popup with the unique name
                // The popup logic will now find the correct ID because the name is unique
                if (promptForDistances({uniquePlaceholder}) == QDialog::Rejected) {
                    qDebug() << "Distance entry cancelled. Deleting" << uniquePlaceholder;

                    // Fetch the ID of the row we just added to ensure clean removal
                    int idToDelete = campusModel->record(rowCount).value("campusId").toInt();
                    removeCampus(idToDelete);
                    refreshUI();
                }
            }
        }
    });
}

void AdminPage::on_uploadFile_clicked() {
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Select Excel File"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Excel Files (*.xlsx *.xls)")
        );

    if (filePath.isEmpty()) return;

    // 1. Perform upload and get the names of NEW campuses
    QStringList added = uploadFileAppend(filePath);

    // 2. Refresh the UI so the models see the new data
    refreshUI();

    // 3. If there were new campuses, prompt for their distances to existing ones
    if (!added.isEmpty()) {
        promptForDistances(added);
    }

    emit notifyStatus("File uploaded and distances configured!");
}

void AdminPage::refreshUI() {
    qDebug() << "AdminPage: Database data re-synced to UI.";
    // 1. Reload the main campus list
    campusModel->select();

    // 2. Figure out which campus was selected before the refresh
    QModelIndex currentIndex = ui->campusList->currentIndex();
    if (currentIndex.isValid()) {
        QSqlRecord record = campusModel->record(currentIndex.row());
        int campusId = record.value("campusId").toInt();

        // 3. Re-apply the filter to the souvenirs so they stay visible
        souvenirModel->setFilter(QString("campusId = %1").arg(campusId));
        souvenirModel->select();
    }
}

int AdminPage::promptForDistances(const QStringList &newCampusNames) {
    if (newCampusNames.isEmpty()) return QDialog::Rejected;

    QDialog detailDialog(this);
    detailDialog.setWindowTitle("Configure Campus Distances");
    detailDialog.setMinimumSize(650, 450);
    detailDialog.setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(&detailDialog);

    // Create the Table
    QTableWidget *distanceTable = new QTableWidget(&detailDialog);
    distanceTable->setColumnCount(3);
    distanceTable->setHorizontalHeaderLabels({"New Campus", "To Existing Campus", "Distance (mi)"});
    distanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 1. Fetch ALL existing campuses directly from the DB to ensure we have the latest data
    struct CampusInfo { int id; QString name; };
    QList<CampusInfo> existingCampuses;

    QSqlQuery query("SELECT campusId, campusName FROM campusList");
    while (query.next()) {
        QString name = query.value(1).toString();
        // We only want to pair the "New" campus with campuses that were ALREADY there
        if (!newCampusNames.contains(name)) {
            existingCampuses.append({query.value(0).toInt(), name});
        }
    }

    // 2. Populate the Table Rows
    // Logic: For every NEW campus, create a row for every EXISTING campus
    int currentRow = 0;
    for (const QString &newName : newCampusNames) {
        for (const auto &ex : existingCampuses) {
            distanceTable->insertRow(currentRow);

            // Column 0: New Campus Name (Read Only)
            QTableWidgetItem *newItem = new QTableWidgetItem(newName);
            newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
            distanceTable->setItem(currentRow, 0, newItem);

            // Column 1: Existing Campus Name (Read Only)
            QTableWidgetItem *exItem = new QTableWidgetItem(ex.name);
            exItem->setData(Qt::UserRole, ex.id); // Store the ID for the database helper
            exItem->setFlags(exItem->flags() & ~Qt::ItemIsEditable);
            distanceTable->setItem(currentRow, 1, exItem);

            // Column 2: Distance Input (Editable)
            QTableWidgetItem *distItem = new QTableWidgetItem("0");
            distItem->setTextAlignment(Qt::AlignCenter);
            distanceTable->setItem(currentRow, 2, distItem);

            currentRow++;
        }
    }

    // 3. Add Buttons
    QPushButton *saveBtn = new QPushButton("Save Distances", &detailDialog);
    saveBtn->setStyleSheet("padding: 8px; font-weight: bold;");
    layout->addWidget(distanceTable);
    layout->addWidget(saveBtn);

    // Connect Save Button to Database Logic
    connect(saveBtn, &QPushButton::clicked, [&]() {
        QSqlQuery idLookup;
        for (int i = 0; i < distanceTable->rowCount(); ++i) {
            QString newName = distanceTable->item(i, 0)->text();
            int targetId = distanceTable->item(i, 1)->data(Qt::UserRole).toInt();
            int dist = distanceTable->item(i, 2)->text().toInt();

            // Find the ID of the New Campus
            idLookup.prepare("SELECT campusId FROM campusList WHERE campusName = :name");
            idLookup.bindValue(":name", newName);

            if (idLookup.exec() && idLookup.next()) {
                int newId = idLookup.value(0).toInt();

                // Use your databaseHelper functions to add edges in both directions
                addDistance(newId, targetId, dist);
                addDistance(targetId, newId, dist);
            }
        }
        detailDialog.accept(); // Closes with QDialog::Accepted
    });

    return detailDialog.exec();
}

void AdminPage::setupLoginPage() {
    // Make pressing enter submit the user info
    connect(ui->usernameField, &QLineEdit::returnPressed, this, &AdminPage::handleLogin);
    connect(ui->passwordField, &QLineEdit::returnPressed, this, &AdminPage::handleLogin);

    // link the login button
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AdminPage::handleLogin);
    // link the cancel button
    connect(ui->buttonBox, &QDialogButtonBox::rejected, [this](){
        ui->usernameField->clear();
        ui->passwordField->clear();
    });
}
