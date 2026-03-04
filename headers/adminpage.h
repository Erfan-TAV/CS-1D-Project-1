#ifndef COLLEGETOUR_ADMINPAGE_H
#define COLLEGETOUR_ADMINPAGE_H

#include <QSqlTableModel>
#include <QWidget>
#include "ui_AdminPage.h"
#include "databasePage.h"


QT_BEGIN_NAMESPACE

namespace Ui
{
    class AdminPage;
}

QT_END_NAMESPACE

class AdminPage : public DatabasePage
{
    Q_OBJECT

public:
    explicit AdminPage(QWidget* parent = nullptr);
    ~AdminPage() override;

private slots:
    void handleLogin();    
    void on_uploadFile_clicked();
    void refreshUI() override;

private:
    Ui::AdminPage* ui;
    QSqlTableModel* campusModel;
    QSqlTableModel* souvenirModel;
    void setupDatabaseTable();

signals:
    void notifyStatus(const QString &message, int timeout = 2500);
};


#endif //COLLEGETOUR_ADMINPAGE_H
