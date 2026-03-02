#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void handleLogin();

    void on_startTripButton_clicked();

    void on_planAnotherButton_clicked();

    void on_planAnotherButton_1_clicked();

    void on_tripPlanStopNextButton_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
