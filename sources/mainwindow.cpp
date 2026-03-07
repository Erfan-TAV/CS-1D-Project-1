#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include "databaseHelper.h"
#include <QStandardPaths>
#include <QDir>
#include <QSqlTableModel>

/*
 * trip planner page indexes
index 0 = home
index 1 = plan
index 2 = stop
index 3 = result
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    linkAdminPage();

    // link menubar item
    connect(ui->actionreset_all_information, &QAction::triggered, this, &MainWindow::menuBarReset);

    // set starting tab to planning tab
    // TODO: change to 0 which is planner page, currently set to 1 for testing info page
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Run this in your constructor or a setup function
void MainWindow::initializeList() {
    // 1. Create the Model
    // 'dbManager' is your instance of DbManager.
    // 'this' ensures the model is deleted when the window closes.
    QSqlTableModel *model = new QSqlTableModel(this, dbHandler->getDatabase());

    // 2. Specify your table name exactly as it appears in the .db file
    model->setTable("your_table_name");

    // 3. (Optional) If you want to sort by a specific column (e.g., Column 0)
    model->setSort(0, Qt::AscendingOrder);

    // 4. Fetch the data from the database into the model
    model->select();

    // 5. Tell the ListView to use this model
    // ui->campusList->setModel(model);

    // 6. Tell the ListView which column to display (0, 1, 2, etc.)
    // For example, if Column 1 is "Campus Name", set it to 1.
    // ui->campusList->setModelColumn(1);
}

void MainWindow::linkAdminPage() {
    // We connect the CUSTOM signal from AdminPage to the BUILT-IN slot of MainWindow
    connect(ui->adminPageWidget, &AdminPage::notifyStatus, this, [this](const QString &msg) {
        // This lambda function runs whenever notifyStatus is emitted
        ui->statusBar->showMessage(msg, 3000); // Show for 3 seconds
    });
}

void MainWindow::menuBarReset() const {
    // 1. Define a universal path in AppData
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString excelPath = appDataPath + "/starterInformation.xlsx";

    qDebug() << "--- Template Debug Info ---";
    qDebug() << "Expected Folder:" << appDataPath;
    qDebug() << "Full excel Path:" << excelPath;

    // 2. If it's not in AppData yet, copy it from the internal resources
    if (!QFile::exists(excelPath)) {
        QDir().mkpath(appDataPath); // Ensure folder exists
        QFile::copy(":/res/starterInformation.xlsx", excelPath);
        // Remove read-only attribute so we can use it
        QFile::setPermissions(excelPath, QFileDevice::WriteOwner | QFileDevice::ReadOwner);
    }

    // 3. Use the local AppData path instead of the C:/Users hardcoded path
    if (QFile::exists(excelPath)) {
        resetAndReloadData(excelPath);

        // Refresh pages...
        QList<DatabasePage*> allPages = this->findChildren<DatabasePage*>();
        for (DatabasePage* page : std::as_const(allPages)) {
            if (page) page->refreshUI();
        }

        ui->statusBar->showMessage("Database Reset Successful", 3000);
    } else {
        ui->statusBar->showMessage("Reset failed: Template file not found", 3000);
    }
}

// mainwindow.cpp
void MainWindow::on_tabWidget_currentChanged(int index) const {
    qDebug() << "Auto-connected Tab Change. Current Index:" << index;

    // We look inside the container (databasePage) for any PlanPage instances
    QList<DatabasePage*> allPages = this->findChildren<DatabasePage*>();

    if (allPages.isEmpty()) {
        qDebug() << "Warning: No PlanPage found inside databasePage!";
    }

    for (DatabasePage* page : allPages) {
        page->refreshUI();
        qDebug() << "Successfully refreshed:" << page->objectName();
    }
}
