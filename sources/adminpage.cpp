#include "adminpage.h"
#include "ui_AdminPage.h"


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
