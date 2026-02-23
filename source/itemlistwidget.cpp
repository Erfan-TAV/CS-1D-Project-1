#include "../headers/itemlistwidget.h"
#include "ui_itemlistwidget.h"

ItemListWidget::ItemListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemListWidget)
{
    ui->setupUi(this);
}

ItemListWidget::~ItemListWidget()
{
    delete ui;
}

void ItemListWidget::setData(const Souvenir &item)
{
    // Sets the name label
    ui->itemName->setText(item.name);

    // Formats the double (price) to 2 decimal places with a '$' sign
    // The 'f' stands for fixed-point notation
    ui->itemPrice->setText(QString("$%1").arg(item.price, 0, 'f', 2));
}
