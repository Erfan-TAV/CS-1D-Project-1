#include "infopage.h"
#include "ui_infopage.h"
#include <QHeaderView>

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
