#include "souvenirwindow.h"
#include "ui_souvenirwindow.h"
#include "collegemanager.h"

#include <QString>

SouvenirWindow::SouvenirWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SouvenirWindow)
    , m_manager(nullptr)
    , m_collegeIndex(-1)
{
    ui->setupUi(this);

    // simple defaults
    setWindowTitle("Souvenirs");
}

SouvenirWindow::~SouvenirWindow()
{
    delete ui;
}

void SouvenirWindow::setData(CollegeManager* manager, int collegeIndex)
{
    m_manager = manager;
    m_collegeIndex = collegeIndex;
    refreshUi();
}

void SouvenirWindow::refreshUi()
{
    if (!m_manager || m_collegeIndex < 0) return;

    // title
    ui->lblCollege->setText("Welcome to " + QString::fromStdString(m_manager->collegeName(m_collegeIndex)));

    // 3 souvenir buttons
    const auto& items = m_manager->souvenirsFor(m_collegeIndex);

    ui->btnItem1->setText("1) " + QString::fromStdString(items[0].name) + " - $" + QString::number(items[0].price, 'f', 2));
    ui->btnItem2->setText("2) " + QString::fromStdString(items[1].name) + " - $" + QString::number(items[1].price, 'f', 2));
    ui->btnItem3->setText("3) " + QString::fromStdString(items[2].name) + " - $" + QString::number(items[2].price, 'f', 2));

    // spent here
    ui->lblSpentHere->setText("Total here: $" + QString::number(m_manager->spentAtCollege(m_collegeIndex), 'f', 2));
}

void SouvenirWindow::buyItem(int souvenirIndex)
{
    if (!m_manager || m_collegeIndex < 0) return;

    int qty = ui->spinQty->value();
    m_manager->buySouvenir(m_collegeIndex, souvenirIndex, qty);

    // update label
    ui->lblSpentHere->setText("Total here: $" + QString::number(m_manager->spentAtCollege(m_collegeIndex), 'f', 2));
}

void SouvenirWindow::on_btnItem1_clicked() { buyItem(0); }
void SouvenirWindow::on_btnItem2_clicked() { buyItem(1); }
void SouvenirWindow::on_btnItem3_clicked() { buyItem(2); }

void SouvenirWindow::on_btnBack_clicked()
{
    close();
}
