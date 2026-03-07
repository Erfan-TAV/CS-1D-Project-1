#include "infopage.h"
#include "ui_infopage.h"
#include <QHeaderView>
#include <QSqlQuery>

InfoPage::InfoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoPage)
{
    ui->setupUi(this);

    // Your Original Header Setup
    QHeaderView *header = ui->tableWidget->horizontalHeader();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->tableWidget->setColumnWidth(2, 80);
    header->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidget->verticalHeader()->setVisible(false);
}

InfoPage::~InfoPage() { delete ui; }

/**
 * Updates the UI with the final recursive path and distance.
 */
void InfoPage::displayTripResults(const TripResult &result)
{
    // Requirement: Accurate display of total distance
    ui->totalDistanceAmount->setText(QString::number(result.totalDistance, 'f', 2) + " mi");

    ui->tableWidget->setRowCount(0);
    for (int i = 0; i < result.campusOrder.size(); ++i) {
        int campusId = result.campusOrder[i];
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        QSqlQuery query;
        query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
        query.bindValue(":id", campusId);
        
        if (query.exec() && query.next()) {
            // Col 0: Name | Col 1: Visit Order | Col 2: ID
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(i + 1)));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(campusId)));
        }
    }
}
