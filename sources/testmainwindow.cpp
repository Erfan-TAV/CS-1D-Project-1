#include "testmainwindow.h"
#include "ui_testmainwindow.h"
#include "adminpage.h"

TestMainWindow::TestMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TestMainWindow)
{
    ui->setupUi(this);

    // We connect the CUSTOM signal from AdminPage to the BUILT-IN slot of MainWindow
    connect(ui->widget, &AdminPage::notifyStatus, this, [this](const QString &msg) {
        // This lambda function runs whenever notifyStatus is emitted
        ui->statusbar->showMessage(msg, 3000); // Show for 3 seconds
    });
}

TestMainWindow::~TestMainWindow()
{
    delete ui;
}
