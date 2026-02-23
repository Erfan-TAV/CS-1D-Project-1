#include "itemlistwidget.h"
#include "ui_itemlistwidget.h"
#include <QIntValidator>

itemListWidget::itemListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::itemListWidget) {
    ui->setupUi(this);
}

itemListWidget::~itemListWidget() {
    delete ui;
}

// todo: make compatible with sql
void itemListWidget::setData(const Souvenir &item) {
    ui->itemName->setText(item.name);
    ui->itemPrice->setText(QString("$%1").arg(item.price, 0, 'f', 2));
}
