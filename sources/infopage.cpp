#include "infopage.h"
#include "tripPlanner.h"
#include "ui_infopage.h"
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include "databaseHelper.h"
#include <QTableWidgetItem>
#include <QDebug>

#include "planpage.h"

InfoPage::InfoPage( QWidget *parent)
    : DatabasePage(parent)
    , ui(new Ui::InfoPage)
{
    ui->setupUi(this);

    // Create the model for the QTableView
    model = new QStandardItemModel(this);

    // Set headers (same as before)
    model->setHorizontalHeaderLabels({ "Campus", "Item", "Quantity", "Price", "Total" });

    ui->tableView->setModel(model);

    // Hide row headers
    ui->tableView->verticalHeader()->setVisible(false);

    // Column resizing
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    header->setSectionResizeMode(3, QHeaderView::Interactive);
    header->setSectionResizeMode(4, QHeaderView::Interactive);
    ui->tableView->setColumnWidth(2, 60);
    ui->tableView->setColumnWidth(3, 100);
    ui->tableView->setColumnWidth(4, 70);
}

InfoPage::~InfoPage() { delete ui; }

/**
 * Updates the UI with the final recursive path and distance.
 */
void InfoPage::displayTripResults(const TripResult &result)
{
    ui->totalDistanceAmount->setText(QString::number(result.totalDistance, 'f', 2) + " mi");

    model->removeRows(0, model->rowCount());

    for (int i = 0; i < result.campusOrder.size(); ++i) {
        int campusId = result.campusOrder[i];

        QSqlQuery query;
        query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
        query.bindValue(":id", campusId);

        if (query.exec() && query.next()) {
            QList<QStandardItem*> rowItems;
            rowItems << new QStandardItem(query.value(0).toString()); // campusName
            rowItems << new QStandardItem(QString::number(i + 1));     // visit order
            rowItems << new QStandardItem(QString::number(campusId));  // campusID

            // For table consistency, add empty items for remaining columns
            rowItems << new QStandardItem(""); // itemPrice placeholder
            rowItems << new QStandardItem(""); // totalPrice placeholder

            model->appendRow(rowItems);
        }
    }
}

void InfoPage::refreshUI()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    model->removeRows(0, model->rowCount()); // clear model

    if (!query.exec("SELECT campusName, itemName, numItem, itemPrice, totalPrice FROM tripInfo")) {
        qDebug() << "InfoPage refresh error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(query.value(0).toString()); // campusName
        rowItems << new QStandardItem(query.value(1).toString()); // itemName
        rowItems << new QStandardItem(QString::number(query.value(2).toInt())); // numItem
        rowItems << new QStandardItem(QString::number(query.value(3).toDouble(), 'f', 2)); // itemPrice
        rowItems << new QStandardItem(QString::number(query.value(4).toDouble(), 'f', 2)); // totalPrice

        model->appendRow(rowItems);
    }

    ui->totalSpentAmount->setText(QString::number(getTripInfoTotalSpent(), 'f', 2));
    ui->totalItemAmount->setText(QString::number(getTripInfoTotalItems()));

    calculateTotalDistance();
}

void InfoPage::calculateTotalDistance() {
    QSqlQuery query;
    double totalDistance = 0.0;

    if (!query.exec("SELECT IFNULL(SUM(tripDistance), 0) FROM tripInfoDistances")) {
        qDebug() << "[DB ERROR] Failed to get total tripDistance:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        totalDistance = query.value(0).toDouble();
    }

    ui->totalDistanceAmount->setText(QString::number(totalDistance, 'f', 2));
}

void InfoPage::resetUI()
{
    model->removeRows(0, model->rowCount());   // clear table
    ui->totalSpentAmount->setText("0.00");
    ui->totalItemAmount->setText("0");
    ui->totalDistanceAmount->setText("0.00");
}
