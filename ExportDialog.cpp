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

    // If the chose file dialog has been canceled return
    if (selectedFilePath.isEmpty())
        return;

    if (!selectedFilePath.split(".").endsWith("png") || !selectedFilePath.split(".").endsWith("PNG"))
        selectedFilePath += ".png";

    QFile file(selectedFilePath);
    file.open(QIODevice::WriteOnly);

    qreal chartViewPixelRatio = this->chartView->devicePixelRatioF();
    QPixmap pixMap(this->chartView->width() * chartViewPixelRatio , this->chartView->height() * chartViewPixelRatio);
    pixMap.setDevicePixelRatio(chartViewPixelRatio);
    pixMap.fill(Qt::transparent);

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
