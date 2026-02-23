#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "souvenirwindow.h"

#include <QString>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_souvenirWin(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("College Tour (Simple)");

    // create souvenir window once
    m_souvenirWin = new SouvenirWindow(this);

    refreshCollegeList();
    refreshStats();
    refreshOffer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshCollegeList()
{
    ui->listColleges->clear();

    // show ONLY unvisited colleges (Saddleback already visited)
    auto available = m_manager.availableColleges();
    for (int idx : available) {
        if (idx == m_manager.currentIndex()) continue;

        double d = m_manager.distance(m_manager.currentIndex(), idx);

        QString line = QString::fromStdString(m_manager.collegeName(idx))
                       + "  (" + QString::number(d, 'f', 1) + " miles)";

        // store real index inside item
        QListWidgetItem* item = new QListWidgetItem(line);
        item->setData(Qt::UserRole, idx);
        ui->listColleges->addItem(item);
    }
}

void MainWindow::refreshStats()
{
    ui->lblCurrent->setText("Current: " + QString::fromStdString(m_manager.collegeName(m_manager.currentIndex())));
    ui->lblMiles->setText("Total Miles: " + QString::number(m_manager.totalMiles(), 'f', 1));
    ui->lblTotal->setText("Grand Total: $" + QString::number(m_manager.grandTotal(), 'f', 2));
}

void MainWindow::refreshOffer()
{
    int offerIdx = m_manager.closestOfferIndex();
    if (offerIdx == -1) {
        ui->lblOffer->setText("Offer: (none)");
        return;
    }

    double d = m_manager.distance(m_manager.currentIndex(), offerIdx);
    ui->lblOffer->setText("Offer (closest): " + QString::fromStdString(m_manager.collegeName(offerIdx))
                          + " - " + QString::number(d, 'f', 1) + " miles");
}

void MainWindow::on_btnVisit_clicked()
{
    QListWidgetItem* cur = ui->listColleges->currentItem();
    if (!cur) {
        QMessageBox::information(this, "Choose", "Please select a college first.");
        return;
    }

    int chosenIndex = cur->data(Qt::UserRole).toInt();

    // travel to that college
    m_manager.visitCollege(chosenIndex);

    // open souvenir window for that college
    m_souvenirWin->setData(&m_manager, chosenIndex);
    m_souvenirWin->exec();  // modal dialog

    // after dialog closes, refresh
    refreshCollegeList();
    refreshStats();
    refreshOffer();

    // if no more colleges, show final message
    if (m_manager.availableColleges().size() <= 1) {
        QMessageBox::information(this, "Done",
                                 "No more colleges left.\n\nTotal Miles: " + QString::number(m_manager.totalMiles(), 'f', 1) +
                                     "\nGrand Total: $" + QString::number(m_manager.grandTotal(), 'f', 2));
    }
}
