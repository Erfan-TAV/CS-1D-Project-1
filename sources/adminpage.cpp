#include "adminpage.h"
#include "ui_AdminPage.h"
#include <qsqlerror.h>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QTimer>


AdminPage::AdminPage(QWidget* parent) :
    QWidget(parent), ui(new Ui::AdminPage)
{
    ui->setupUi(this);

    // set the page to login screen by default
    ui->adminPageStack->setCurrentIndex(0);

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

    setupDatabaseTable();
}

AdminPage::~AdminPage()
{
    delete ui;
}

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

        // 1. Get the Campus ID (Column 0) and Name (Column 1)
        int idToDelete = campusModel->data(campusModel->index(currentIndex.row(), 0)).toInt();
        QString name = campusModel->data(campusModel->index(currentIndex.row(), 1)).toString();

        // 2. Direct SQL Delete to avoid model caching bugs
        QSqlQuery query;
        query.prepare("DELETE FROM campusList WHERE campusID = :id");
        query.bindValue(":id", idToDelete);

        if (query.exec()) {
            // 3. Force the model to reload from the disk
            campusModel->select();

            // 4. Clear the LineEdit and Souvenir table since that campus is gone
            ui->collegeNameLineEdit->clear();
            souvenirModel->setFilter("campusID = -1"); // Filter to nothing
            souvenirModel->select();

            qDebug() << "Deleted campus:" << name;
        } else {
            qDebug() << "Delete failed:" << query.lastError().text();
        }
    });
    connect(ui->addCampusButton, &QPushButton::clicked, this, [this]() {
        // 1. Insert a new row at the bottom
        int rowCount = campusModel->rowCount();

        if (campusModel->insertRow(rowCount)) {
            // 2. Set a placeholder name so the row isn't invisible/empty
            // SQLite will auto-generate the campusID if set to AUTOINCREMENT
            campusModel->setData(campusModel->index(rowCount, 1), "New College");

            // 3. Submit to database to generate the ID
            if (campusModel->submitAll()) {
                // 4. Select and focus the new item for immediate renaming
                QModelIndex newIndex = campusModel->index(rowCount, 1);
                ui->campusList->setCurrentIndex(newIndex);

                // Focus the line edit so the user can start typing the real name
                ui->collegeNameLineEdit->setText("New College");
                ui->collegeNameLineEdit->setFocus();
                ui->collegeNameLineEdit->selectAll();
            }
        }
        // TODO: add logic to add distances from the newly added campus to all the current campus
        qDebug() << "failed to add updated distances.";
    });

}
