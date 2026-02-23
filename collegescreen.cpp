#include "collegescreen.h"
#include "./ui_collegescreen.h"
#include "itemlistwidget.h"
#include <QListWidgetItem>

collegeScreen::collegeScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::collegeScreen)
{
    ui->setupUi(this);

    // Connect the Cancel button to close the window
    connect(ui->purchaseConfirm, &QDialogButtonBox::rejected, this, &QWidget::close);
}

collegeScreen::~collegeScreen()
{
    delete ui;
}

void collegeScreen::setCampusName(const QString &Name) {
    ui->campusName->setText(Name);
}

void collegeScreen::populateList(const QList<Souvenir>& items) {
    ui->listWidget->clear();

    for (const Souvenir& item : items) {
        // Create the custom row widget
        ItemListWidget *rowWidget = new ItemListWidget(this);
        rowWidget->setData(item);

        // Create the container for the ListWidget
        QListWidgetItem *itemContainer = new QListWidgetItem(ui->listWidget);
        itemContainer->setSizeHint(QSize(0, 60));

        ui->listWidget->addItem(itemContainer);
        ui->listWidget->setItemWidget(itemContainer, rowWidget);
    }
}
