#ifndef COLLEGESCREEN_H
#define COLLEGESCREEN_H

#include <QMainWindow>
#include "tempInfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class collegeScreen; }
QT_END_NAMESPACE

class collegeScreen : public QMainWindow
{
    Q_OBJECT

public:
    collegeScreen(QWidget *parent = nullptr);
    ~collegeScreen();

    void setCampusName(const QString &Name);
    void populateList(const QList<Souvenir>& items);

private:
    Ui::collegeScreen *ui;
};
#endif // COLLEGESCREEN_H
