#ifndef PLANPAGE_H
#define PLANPAGE_H

#include <QSqlTableModel>
#include <QWidget>
#include "databasePage.h"

namespace Ui {
    class PlanPage;
}
// TODO: add doxy
class PlanPage : public DatabasePage
{
    Q_OBJECT

public:
    explicit PlanPage(QWidget *parent = nullptr);
    ~PlanPage() override;

private:
    Ui::PlanPage *ui;
    void setupDatabaseTable();
    QSqlTableModel* campusModel;     /**< Model for the campus selection list on settings page. */
    QSqlTableModel* comboBoxModel;   /**< Model for the combobox on settings page. */
    QSqlTableModel* tripModel;       /**< Model for the current trip table */

private slots:
    void on_startTripButton_clicked();
    void on_planAnotherButton_clicked();
    void on_planAnotherButton_1_clicked();
    void on_tripPlanStopNextButton_clicked();
    void updateFilteredTable(const QString &selectedCampus);
    void on_pushButton_7_clicked();

    void refreshUI() override;

signals:

};

#endif // PLANPAGE_H
