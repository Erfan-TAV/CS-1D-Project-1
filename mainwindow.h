#pragma once

#include <QMainWindow>
#include "collegemanager.h"

class SouvenirWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnVisit_clicked();

private:
    void refreshCollegeList();
    void refreshStats();
    void refreshOffer();

    Ui::MainWindow *ui;

    CollegeManager m_manager;
    SouvenirWindow* m_souvenirWin;   // created once, reused
};
