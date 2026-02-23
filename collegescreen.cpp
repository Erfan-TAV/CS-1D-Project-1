#include "collegescreen.h"
#include "./ui_collegescreen.h"
#include <QStringListModel>
#include "itemlistwidget.h"

collegeScreen::collegeScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::collegeScreen)
{
    ui->setupUi(this);

    connect(ui->purchaseConfirm, &QDialogButtonBox::rejected, this, &QWidget::close);
}

collegeScreen::~collegeScreen()
{
    delete ui;
}

void collegeScreen::setCampusName(const QString &Name) {
    ui->campusName->setText(Name);
}

// todo: make compatible with database
void collegeScreen::populateList(const QList<Souvenir>& items) {
    ui->listWidget->clear();

    for (const Souvenir& item : items) {
        itemListWidget *rowWidget = new itemListWidget(this);
        rowWidget->setData(item); // Pass the whole struct

        QListWidgetItem *itemContainer = new QListWidgetItem(ui->listWidget);
        itemContainer->setSizeHint(QSize(0, 60));

        ui->listWidget->addItem(itemContainer);
        ui->listWidget->setItemWidget(itemContainer, rowWidget);
    }
}
