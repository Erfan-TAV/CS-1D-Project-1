#ifndef COLLEGETOUR_ADMINPAGE_H
#define COLLEGETOUR_ADMINPAGE_H

#include <QSqlTableModel>
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
    void handleFilUpload();

private:
    Ui::AdminPage* ui;
    QSqlTableModel* campusModel;
    QSqlTableModel* souvenirModel;
    void setupDatabaseTable();

signals:
    void notifyStatus(const QString &message, int timeout = 2500);
};


#endif //COLLEGETOUR_ADMINPAGE_H
