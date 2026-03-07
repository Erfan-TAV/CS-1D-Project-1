#include "planpage.h"

#include "ui_planpage.h"



PlanPage::PlanPage(QWidget *parent)

    : QWidget(parent)

    , ui(new Ui::PlanPage)

{

    ui->setupUi(this);



    // set the starting page to the plan setting page

    ui->tripPlannerStack->setCurrentIndex(0);



    // ------------------------------------------------------------------------------------

    // setup the table in tripPlan

    // Set the first column (Campus Name) to stretch and fill the table

    ui->campusTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    // Hide row headers

    // ui->tableWidget->verticalHeader()->setVisible(false);

    // ------------------------------------------------------------------------------------

    // setup the table in tripPlan

}



PlanPage::~PlanPage()

{

    delete ui;

}



void PlanPage::on_startTripButton_clicked()

{

    // ui->tabWidget->setCurrentIndex(0);



    if (ui->planOnlyCheckBox->isChecked()) {

        ui->tripPlannerStack->setCurrentIndex(1);

    } else {

        ui->tripPlannerStack->setCurrentIndex(2);

    }

}





void PlanPage::on_planAnotherButton_clicked()

{

    ui->tripPlannerStack->setCurrentIndex(0);



    // TODO: setup logic to prepare program for another trip plan.

}





void PlanPage::on_planAnotherButton_1_clicked()

{

    ui->tripPlannerStack->setCurrentIndex(0);

}





void PlanPage::on_tripPlanStopNextButton_clicked()

{

    // TODO: setup logic so that

    ui->tripPlannerStack->setCurrentIndex(3);

}
