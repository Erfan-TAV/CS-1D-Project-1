#include "mainwindow.h"
#include "ui_mainwindow.h"
// #include "dbManager.h"

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

    // ------------------------------------------------------------------------------------
    // set starting tab to planning tab
    // TODO: change to 0 which is planner page, currently set to 1 for testing info page
    ui->tabWidget->setCurrentIndex(0);
    // set adminTab to the admin login page
    // TODO: change to 0 which is login page. currently set to 1 for testing the actual admin page
    ui->adminPageStack->setCurrentIndex(0);
    // set planTab to the home page
    // TODO: ensure its set to 0 if !=0 for testing purposes
    ui->tripPlannerStack->setCurrentIndex(0);
    // ------------------------------------------------------------------------------------


    // ------------------------------------------------------------------------------------
    // Setup the login page
    // Make pressing enter submit the user info
    connect(ui->usernameField, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);
    connect(ui->passwordField, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);

    // link the login button
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::handleLogin);
    // link the cancel button
    connect(ui->buttonBox, &QDialogButtonBox::rejected, [this](){
        ui->usernameField->clear();
        ui->passwordField->clear();
    });
    // ------------------------------------------------------------------------------------
    // Setup the login page

    // ------------------------------------------------------------------------------------
    // setup the table in info tab
    // TODO: setup logic to expand column width for column items that might expand
    QHeaderView *header = ui->tableWidget->horizontalHeader();

    // 1. Disable "Stretch Last Section" so Column 2 doesn't become giant
    header->setStretchLastSection(false);

    // 2. Set Columns 0 and 1 to Stretch mode (they will share the extra space)
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Stretch);

    // 3. Set Column 2 to Interactive (or Fixed) and give it your specific width
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->tableWidget->setColumnWidth(2, 80);

    // 4. Maintenance: allow the window to still shrink
    header->setMinimumSectionSize(10);
    ui->tableWidget->setMinimumWidth(0);
    ui->tableWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    // 5. Centering and hiding row numbers
    header->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidget->verticalHeader()->setVisible(false);
    // ------------------------------------------------------------------------------------
    // setup the table in info tab

    // ------------------------------------------------------------------------------------
    // setup the table in tripPlan
    // Set the first column (Campus Name) to stretch and fill the table
    ui->campusTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    // Hide row headers
    ui->tableWidget->verticalHeader()->setVisible(false);
    // ------------------------------------------------------------------------------------
    // setup the table in tripPlan
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleLogin()
{
    // Access the fields directly from the ui pointer
    QString user = ui->usernameField->text();
    QString pass = ui->passwordField->text();

    // Simple credential check
    // TODO: add proper account checkign logic
    if (user == "admin" && pass == "admin") {
        qDebug() << "Login Successful";
        ui->statusBar->showMessage("Login Successful", 2500);

        // Clear fields for security
        ui->usernameField->clear();
        ui->passwordField->clear();

        // Switch the stack to the Admin Dashboard (Index 0)
        ui->adminPageStack->setCurrentIndex(1);
    } else {
        qDebug() << "Login Failed";
        ui->statusBar->showMessage("Login Failed", 2500);
    }
}

void MainWindow::on_startTripButton_clicked()
{
    // ui->tabWidget->setCurrentIndex(0);

    if (ui->planOnlyCheckBox->isChecked()) {
        ui->tripPlannerStack->setCurrentIndex(1);
    } else {
        ui->tripPlannerStack->setCurrentIndex(2);
    }
}


void MainWindow::on_planAnotherButton_clicked()
{
    ui->tripPlannerStack->setCurrentIndex(0);

    // TODO: setup logic to prepare program for another trip plan.
}


void MainWindow::on_planAnotherButton_1_clicked()
{
    ui->tripPlannerStack->setCurrentIndex(0);
}


void MainWindow::on_tripPlanStopNextButton_clicked()
{
    // TODO: setup logic so that
    ui->tripPlannerStack->setCurrentIndex(3);
}

#include <QSqlTableModel>

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
    ui->campusList->setModel(model);

    // 6. Tell the ListView which column to display (0, 1, 2, etc.)
    // For example, if Column 1 is "Campus Name", set it to 1.
    ui->campusList->setModelColumn(1);
}