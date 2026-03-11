#ifndef INFOPAGE_H
#define INFOPAGE_H

#include "databasePage.h"
#include "planpage.h"
#include <QStandardItemModel>
#include "tripPlanner.h"

namespace Ui {
    class InfoPage;
}

class InfoPage : public DatabasePage
{
    Q_OBJECT

public:
    explicit InfoPage(QWidget *parent = nullptr);
    ~InfoPage();
    void displayTripResults(const TripResult &result);
    void calculateTotalDistance();
    void resetUI();

private:
    Ui::InfoPage *ui;
    QStandardItemModel* model;

private slots:
    void refreshUI() override;
};

#endif // INFOPAGE_H
