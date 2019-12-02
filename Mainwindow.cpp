#include "Mainwindow.h"
#include "ui_Mainwindow.h"

#include <QVector>
#include <QPair>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plotHeatMap() {
    QCPColorMap * colorMap = new QCPColorMap(ui->plotWidget->xAxis, ui->plotWidget->yAxis);

//    QCPColorMapData colorMapData(10, 10);

    colorMap->setGradient(QCPColorGradient::gpPolar);
    colorMap->rescaleDataRange(true);
    colorMap->setInterpolate(false);
    ui->plotWidget->rescaleAxes();
    ui->plotWidget->replot();
}

void MainWindow::plotHeatMap(QVector<QVector<QPair<QString, double>>> tissueCorrelations) {
    QCPColorMap * colorMap = new QCPColorMap(ui->plotWidget->xAxis, ui->plotWidget->yAxis);

    colorMap->data()->setSize(tissueCorrelations.length(), tissueCorrelations[0].length());
    colorMap->data()->setRange(QCPRange(0, tissueCorrelations.length()), QCPRange(0, tissueCorrelations[0].length()));

    QSharedPointer<QCPAxisTickerText> textTickerX(new QCPAxisTickerText);
    QSharedPointer<QCPAxisTickerText> textTickerY(new QCPAxisTickerText);
    textTickerX->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    textTickerY->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);

    for (int i = tissueCorrelations.length() - 1; i >= 0; i--) {
        textTickerX->addTick(i, QString("Cluster ").append(QString::number(i + 1)));
    }
    for (int i = tissueCorrelations[0].length() - 1; i >= 0; i--) {
        QString tissueID = tissueCorrelations[0][i].first;
        textTickerY->addTick(tissueCorrelations[0].length() - i, tissueID);
    }

    // Label the axes
    colorMap->keyAxis()->setTicker(textTickerX);
    colorMap->valueAxis()->setTicker(textTickerY);

//    colorMap->selectionDecorator()->drawDecoration();

//    for (int i = 0; i < tissueCorrelations[0].length(); i++) {
//        double correlation = tissueCorrelations[0][i].second;
//        qDebug() << correlation;
//        colorMap->data()->setCell(0, i, correlation);
//    }

//    for (int i = 0; i < tissueCorrelations[a].length(); i++) {
//        double correlation = tissueCorrelations[a][i].second;
//        qDebug() << correlation;
//        colorMap->data()->setCell(a, i, correlation);
//    }

    for (int i = 0; i < tissueCorrelations.length(); i++) {
        for (int j = 0; j < tissueCorrelations[i].length(); j++) {
//            qDebug() << "Cell at " << i << j << ":" << tissueCorrelations[i][j].first << ":" << tissueCorrelations[i][j].second;
            colorMap->data()->setCell(i, j, tissueCorrelations[i][j].second);
        }
    }

    colorMap->setGradient(QCPColorGradient::gpPolar);
    colorMap->rescaleDataRange(true);
    colorMap->setInterpolate(false);
    colorMap->setAntialiased(false);
//    colorMap->setAntialiasedFill(true);
//    colorMap->setAntialiasedScatters(false);
    ui->plotWidget->rescaleAxes();
    ui->plotWidget->replot();

//    QCPColorMap * colorMap = new QCPColorMap(ui->plotWidget->xAxis, ui->plotWidget->yAxis);

//    int range = 5;
//    colorMap->data()->setSize(5, 5);
//    colorMap->data()->setRange(QCPRange(0, 5), QCPRange(0, 5));

//    colorMap->data()->setData(0, 0, 1);
//    colorMap->data()->setRange(QCPRange(0, tissueCorrelations.length()), QCPRange(0, tissueCorrelations[0].length()));
//    colorMap->data()->setRange();

//    QSharedPointer<QCPAxisTickerText> textTickerX(new QCPAxisTickerText);
//    QSharedPointer<QCPAxisTickerText> textTickerY(new QCPAxisTickerText);
//    for (int i = tissueCorrelations.length() - 1; i >= 0; i--)
//        textTickerX->addTick(i, QString("Cluster ").append(QString::number(i + 1)));
//    for (int i = tissueCorrelations[0].length() - 1; i >= 0; i--)
//        textTickerY->addTick(i, tissueCorrelations[0][i].first);

//    // Label the axes
//    colorMap->keyAxis()->setTicker(textTickerX);
//    colorMap->valueAxis()->setTicker(textTickerY);

//    for (int i = 0; i < tissueCorrelations.length(); i++) {
//        for (int j = 0; j < tissueCorrelations[i].length(); j++) {
//            qDebug() << "Cluster:" << i << tissueCorrelations[i][j].first << ":" << tissueCorrelations[i][j].second;
//            colorMap->data()->setCell(i, j, tissueCorrelations[i][j].second);
//        }
//    }

//    colorMap->setGradient(QCPColorGradient::gpPolar);
//    colorMap->setInterpolate(false);
//    colorMap->rescaleDataRange(true);
////    colorMap->setTightBoundary(true);
//    ui->plotWidget->rescaleAxes();
//    ui->plotWidget->replot();
#if 0
    ui->plotWidget->removePlottable(colorMap);
#endif
}

