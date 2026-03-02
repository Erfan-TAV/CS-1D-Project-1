#ifndef COLLEGETOUR_ADMINPAGE_H
#define COLLEGETOUR_ADMINPAGE_H

#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui
{
    class AdminPage;
}

QT_END_NAMESPACE

class AdminPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminPage(QWidget* parent = nullptr);
    ~AdminPage() override;

private slots:
    void handleLogin();

private:
    Ui::AdminPage* ui;

signals:
    void notifyStatus(const QString &message, int timeout = 2500);
};


#endif //COLLEGETOUR_ADMINPAGE_H
