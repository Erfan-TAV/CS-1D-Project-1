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
    QSqlQueryModel* campusModel = nullptr;     /**< Model for the campus selection list on settings page. */
    QSqlTableModel* comboBoxModel = nullptr;   /**< Model for the combobox on settings page. */
    QSqlTableModel* tripModel = nullptr;       /**< Model for the current trip table */
    QSqlTableModel* tripSouvenirModel = nullptr;
    void setupResultsConnection();
    QWidget* createCampusWidget(QString name, QString distance, bool isLast = false);
    // planonly page
    void renderTrip();
    QWidget* createStopWidget(QString name, int distance, bool showArrow);
    QSortFilterProxyModel* proxyModel;

private slots:
    void on_startTripButton_clicked();
    void on_resultPlanAnotherButton_clicked();
    void on_planOnlyPlanAnotherButton_clicked();
    void on_tripPlanStopNextButton_clicked();
    void updateFilteredTable(const QString &selectedCampus);
    void updateSouvenirFilter(int index);
    void updateSelectionCount();

    void refreshUI() override;

signals:

};

#endif // PLANPAGE_H
