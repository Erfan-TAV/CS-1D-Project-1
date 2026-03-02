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

    linkAdminPage();

    // ------------------------------------------------------------------------------------
    // set starting tab to planning tab
    // TODO: change to 0 which is planner page, currently set to 1 for testing info page
    ui->tabWidget->setCurrentIndex(0);
    // set adminTab to the admin login page
    // TODO: change to 0 which is login page. currently set to 1 for testing the actual admin page
    // ui->adminPageStack->setCurrentIndex(0);
    // set planTab to the home page
    // TODO: ensure its set to 0 if !=0 for testing purposes
    // ------------------------------------------------------------------------------------


    // ------------------------------------------------------------------------------------
    // Setup the login page
    // Make pressing enter submit the user info
    // connect(ui->usernameField, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);
    // connect(ui->passwordField, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);

    // link the login button
    // connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::handleLogin);
    // link the cancel button
    // connect(ui->buttonBox, &QDialogButtonBox::rejected, [this](){
        // ui->usernameField->clear();
        // ui->passwordField->clear();
    // });
    // ------------------------------------------------------------------------------------
    // Setup the login page



}

MainWindow::~MainWindow()
{
    delete ui;
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
