#ifndef PLANPAGE_H
#define PLANPAGE_H

#include <QSqlTableModel>
#include <QWidget>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QRegularExpression>
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
    QSqlQueryModel* campusModel;     /**< Model for the campus selection list on settings page. */
    QSqlTableModel* comboBoxModel;   /**< Model for the combobox on settings page. */
    QSqlTableModel* tripModel;       /**< Model for the current trip table */
    QSqlTableModel* tripSouvenirModel;
    void setupResultsConnection();
    QSortFilterProxyModel* proxyModel;

private slots:
    void on_startTripButton_clicked();
    void on_resultPlanAnotherButton_clicked();
    void on_planAnotherButton_1_clicked();
    void on_tripPlanStopNextButton_clicked();
    void updateFilteredTable(const QString &selectedCampus);
    void updateSouvenirFilter(int index);

    void refreshUI() override;

signals:

};

#endif // PLANPAGE_H
