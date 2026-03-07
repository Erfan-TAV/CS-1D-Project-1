#ifndef INFOPAGE_H
#define INFOPAGE_H

#include <QWidget>

#include "tripPlanner.h"

namespace Ui {
    class InfoPage;
}

class InfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit InfoPage(QWidget *parent = nullptr);
    ~InfoPage();
    void displayTripResults(const TripResult &result);

private:
    Ui::InfoPage *ui;
};

#endif // INFOPAGE_H
