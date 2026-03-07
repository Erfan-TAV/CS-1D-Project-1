#ifndef INFOPAGE_H
#define INFOPAGE_H

#include <QWidget>

#include "databasePage.h"
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

private:
    Ui::InfoPage *ui;

private slots:
    void refreshUI() override;
};

#endif // INFOPAGE_H
