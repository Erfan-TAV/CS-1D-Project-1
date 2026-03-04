#include "infopage.h"
#include "ui_infopage.h"
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

InfoPage::InfoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoPage)
{
    ui->setupUi(this);

    // ------------------------------------------------------------------------------------
    // setup the table in info tab
    // TODO: setup logic to expand column width for column items that might expand
    QHeaderView *header = ui->tableWidget->horizontalHeader();

    // 1. Disable "Stretch Last Section" so Column 2 doesn't become giant
    header->setStretchLastSection(false);

    // 2. Set Columns 0 and 1 to Stretch mode (they will share the extra space)
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Stretch);

    // 3. Set Column 2 to Interactive (or Fixed) and give it your specific width
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->tableWidget->setColumnWidth(2, 80);

    // 4. Maintenance: allow the window to still shrink
    header->setMinimumSectionSize(10);
    ui->tableWidget->setMinimumWidth(0);
    ui->tableWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    // 5. Centering and hiding row numbers
    header->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidget->verticalHeader()->setVisible(false);
    // ------------------------------------------------------------------------------------
    // setup the table in info tab
}

InfoPage::~InfoPage()
{
    delete ui;
}

/**
 * @brief Agile Requirement: Check if the display is accurate
 * This function is called after the recursive algorithm finishes.
 * It maps the trip results to your specific UI labels and table.
 */
void InfoPage::displayTripResults(const TripResult &result)
{
    // 1. Update the Total Distance label (from infopage.ui)
    // Uses 'f' 2 to show two decimal places for mileage
    ui->totalDistanceAmount->setText(QString::number(result.totalDistance, 'f', 2) + " miles");

    // 2. Clear the table and prepare to show the optimized order
    ui->tableWidget->setRowCount(0);

    // 3. Populate the table based on the recursive order determined by the planner
    for (int i = 0; i < result.campusOrder.size(); ++i) {
        int campusId = result.campusOrder[i];
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // Fetch name from database based on the ID in the sequence
        QSqlQuery query;
        query.prepare("SELECT campusName FROM campusList WHERE campusID = :id");
        query.bindValue(":id", campusId);
        
        if (query.exec() && query.next()) {
            QString name = query.value(0).toString();
            
            // Col 0: Campus Name
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
            // Col 1: Visit Order (1, 2, 3...)
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(i + 1)));
            // Col 2: ID (optional, useful for debugging)
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(campusId)));
        }
    }
}
