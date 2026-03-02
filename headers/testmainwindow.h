#ifndef TESTMAINWINDOW_H
#define TESTMAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class TestMainWindow; }
QT_END_NAMESPACE

class TestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestMainWindow(QWidget *parent = nullptr);
    ~TestMainWindow();

private:
    Ui::TestMainWindow *ui;
};

#endif // TESTMAINWINDOW_H
