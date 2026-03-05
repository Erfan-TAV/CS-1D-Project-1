#ifndef PLANPAGE_H
#define PLANPAGE_H

#include <QSqlTableModel>
#include <QWidget>

#include "databasePage.h"
#include "tripPlanner.h"

namespace Ui {
class PlanPage;
}

class PlanPage : public DatabasePage
{
    Q_OBJECT

public:
    explicit PlanPage(QWidget *parent = nullptr);
    ~PlanPage() override;

private:
    Ui::PlanPage *ui;
    void setupDatabaseTable();
    QSqlTableModel* campusModel;

private slots:
    void on_startTripButton_clicked();
    void on_planAnotherButton_clicked();
    void on_planAnotherButton_1_clicked();
    void on_tripPlanStopNextButton_clicked();

    void refreshUI() override;

signals:
    // void tripCalculationFinished(const TripResult &result);

};

#endif // PLANPAGE_H
