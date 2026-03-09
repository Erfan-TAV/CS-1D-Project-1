#ifndef PLANPAGE_H
#define PLANPAGE_H

#include <QSqlTableModel>
#include <QWidget>
#include "databasePage.h"
#include "databaseHelper.h"
#include <QSortFilterProxyModel>

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
    QSortFilterProxyModel* proxyModel; // For performant searching
    QSqlQueryModel* saddlebackModel;   // For the scrollable distance list

private slots:
    void on_startTripButton_clicked();
    void clearHorizontalLayout();
    void on_planAnotherButton_clicked();
    void on_planAnotherButton_1_clicked();
    void on_tripPlanStopNextButton_clicked();
    void updateFilteredTable(const QString &selectedCampus);
    void addTripCampus(int id, QString campusName);
    void refreshUI() override;
    void on_searchLineEdit_textChanged(const QString &text);


signals:

};

#endif // PLANPAGE_H
