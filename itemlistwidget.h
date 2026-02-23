#ifndef ITEMLISTWIDGET_H
#define ITEMLISTWIDGET_H

#include <QWidget>
#include "tempInfo.h"

namespace Ui {
class itemListWidget;
}

class itemListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit itemListWidget(QWidget *parent = nullptr);
    ~itemListWidget();
    void setData(const Souvenir &item);
    // void setItemName(const QString &name);


private slots:


private:
    Ui::itemListWidget *ui;
};

#endif // ITEMLISTWIDGET_H
