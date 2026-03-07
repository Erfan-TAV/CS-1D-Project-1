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
    void initializeList();
    void linkAdminPage();
    void menuBarReset();

signals:
    void databaseChanged();


private:
    Ui::MainWindow *ui;
    DbManager *dbHandler;

};
#endif // MAINWINDOW_H
