#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dbManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startTripButton_clicked();

    void on_planAnotherButton_clicked();

    void on_planAnotherButton_1_clicked();

    void on_tripPlanStopNextButton_clicked();

    void initializeList();

    void linkAdminPage();


private:
    Ui::MainWindow *ui;
    DbManager *dbHandler;

};
#endif // MAINWINDOW_H
