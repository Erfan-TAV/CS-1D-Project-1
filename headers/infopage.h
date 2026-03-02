#ifndef INFOPAGE_H
#define INFOPAGE_H

#include <QWidget>

namespace Ui {
    class InfoPage;
}

class InfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit InfoPage(QWidget *parent = nullptr);
    ~InfoPage();

private:
    Ui::InfoPage *ui;
};

#endif // INFOPAGE_H
