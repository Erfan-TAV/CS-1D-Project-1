#ifndef ITEMLISTWIDGET_H
#define ITEMLISTWIDGET_H

#include <QWidget>
#include "tempInfo.h"

namespace Ui {
class ItemListWidget;
}

class ItemListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemListWidget(QWidget *parent = nullptr);
    ~ItemListWidget();

    // The core function to fill the labels with data
    void setData(const Souvenir &item);

private:
    Ui::ItemListWidget *ui;
};

#endif // ITEMLISTWIDGET_H
