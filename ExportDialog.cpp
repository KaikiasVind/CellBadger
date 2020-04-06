#include "ExportDialog.h"
#include "ui_ExportDialog.h"

#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
#include <QDebug>

#include "Utils/Helper.h"

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
    this->chartView = chartView;
    this->ui->verticalLayoutMain->insertWidget(0, chartView);
}


/**
 * @brief ExportDialog::on_pushButtonSave_clicked
 */
void ExportDialog::on_pushButtonSave_clicked() {
    QString selectedFilePath = Helper::saveFileDialog(this, "png");

    if (!selectedFilePath.split(".").endsWith("png") || !selectedFilePath.split(".").endsWith("PNG"))
        selectedFilePath += ".png";

    QFile file(selectedFilePath);

    QPixmap pixMap(this->chartView->size());
    file.open(QIODevice::WriteOnly);
    QPainter painter(& pixMap);
    painter.setRenderHint(QPainter::Antialiasing);
    this->chartView->render(&painter);
    pixMap.save(&file, "PNG");
}


/**
 * @brief ExportDialog::on_pushButtonBack_clicked
 */
void ExportDialog::on_pushButtonBack_clicked() {
    this->hide();
    this->~ExportDialog();
}
