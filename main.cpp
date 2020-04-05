#include <QApplication>
#include <QDir>
#include <QObject>

#include "Mainwindow.h"
#include "StartDialog.h"
#include "TabWidget.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "System/Coordinator.h"
#include "System/InformationCenter.h"

#include <QtCharts/QSplineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

using QtCharts::QSplineSeries;
using QtCharts::QChart;
using QtCharts::QChartView;

#define run 1

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

#if !run
    QSplineSeries * series = new QSplineSeries();
    series->setName("spline");

    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);

    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Simple spline chart example");
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, 10);

    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(400, 300);
    window.show();

#endif
#if run
    // Declaration of the used widgets
    MainWindow mainWindow;
    StartDialog startDialog;

    // Start of the main software - The start dialog is the first that is shown to the user
    startDialog.show();

    // ++++++++++++++++++++++++++++++++++++++++  CHECK FOR CONFIG FILE ++++++++++++++++++++++++++++++++++++++++
    QString configFilePath = QDir::homePath().append("/.badger.conf");
    ConfigFile configFile;

    bool isConfigFileExits = ConfigFileOperator::isConfigFileExists(configFilePath);

    if (isConfigFileExits) {
        configFile = ConfigFileOperator::readConfigFile(configFilePath);
        bool isDefaultMarkerFileExists = ConfigFileOperator::isFileExists(configFile.cellMarkersFilePath);

        if (!isDefaultMarkerFileExists) {
            startDialog.disableUseDefaultButton();
        }

    } else {
        ConfigFileOperator::createConfigFile(configFilePath);
        configFile = ConfigFileOperator::initializeConfigFile();
    }

    // ++++++++++++++++++++++++++++++++++++++++  CREATE PROGRAM BASICS  ++++++++++++++++++++++++++++++++++++++++

    // InformationCenter is used to store all relevant software data
    InformationCenter informationCenter(configFile);
    // Coordinator is used to control the main data flow
    Coordinator coordinator(informationCenter);


    // +++++++++++++++++++++++++++++++++++++  BUILD SIGNAL AND SLOT LOGIC  +++++++++++++++++++++++++++++++++++++
    // StartDialog -> Coordinator
    QObject::connect(&startDialog, &StartDialog::runNewProject, &coordinator, &Coordinator::on_newProjectStarted);

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedFileParsing, &mainWindow, &MainWindow::on_clusterFileParsed);

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedCorrelating, &mainWindow, &MainWindow::on_correlatingFinished);

    // At this point, the complete control over the system workflow is handed over to the Coordinator

#endif
    return application.exec();
}
