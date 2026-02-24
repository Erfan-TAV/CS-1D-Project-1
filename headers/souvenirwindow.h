#pragma once

#include <QDialog>

class CollegeManager;   // forward declare

namespace Ui {
class SouvenirWindow;
}

class SouvenirWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SouvenirWindow(QWidget *parent = nullptr);
    ~SouvenirWindow();

    void setData(CollegeManager* manager, int collegeIndex);

private slots:
    void on_btnItem1_clicked();
    void on_btnItem2_clicked();
    void on_btnItem3_clicked();
    void on_btnBack_clicked();

private:
    void refreshUi();
    void buyItem(int souvenirIndex);

    Ui::SouvenirWindow *ui;
    CollegeManager* m_manager;
    int m_collegeIndex;
};

