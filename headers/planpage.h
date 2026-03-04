#ifndef PLANPAGE_H
#define PLANPAGE_H

#include <QWidget>

namespace Ui {
class PlanPage;
}

class PlanPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlanPage(QWidget *parent = nullptr);
    ~PlanPage();

private:
    Ui::PlanPage *ui;

private slots:
    void on_startTripButton_clicked();
    void on_planAnotherButton_clicked();
    void on_planAnotherButton_1_clicked();
    void on_tripPlanStopNextButton_clicked();

    signals:

};

#endif // PLANPAGE_H
