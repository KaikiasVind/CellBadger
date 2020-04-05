#include "ExportDialog.h"
#include "ui_ExportDialog.h"

#include <QDebug>

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    // Remove default window frame
//    this->setWindowFlags(Qt::FramelessWindowHint);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}


/**
 * @brief ExportDialog::addPlot
 * @param chartView
 */
void ExportDialog::addPlot(QChartView * chartView) {
    this->ui->verticalLayoutMain->addWidget(chartView);
}
