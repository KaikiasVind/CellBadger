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

#include <QDebug>

#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#include "Utils/Helper.h"
#include "Utils/Math.h"

#include "Utils/Models/AnalysisConfigModel.h"
#include "Utils/Definitions.h"

// Only for graphs
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QLegend>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
using QtCharts::QChart;
using QtCharts::QChartView;
using QtCharts::QLegend;
using QtCharts::QScatterSeries;
using QtCharts::QValueAxis;

#define gui 1

int main(int argc, char * argv[])
{
    QApplication application(argc, argv);

#if !gui

//    QString samplesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/Pbmc_expression.csv";
//    QString cellTypesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/PanglaoDB_markers.csv";

//    QString samplesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\10xGenomics\\Pbmc_expression.csv";
//    QString cellTypesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\10xGenomics\\PanglaoDB_markers.csv";

//    qDebug() << "Parsing.";
//    QVector<FeatureCollection> samples = CSVReader::read10xGenomicsClustersFromFile(samplesFilePath, {15, 0});
//    QVector<FeatureCollection> cellTypes = CSVReader::readCellTypesFromPanglaoDBFile(cellTypesFilePath, {});
//    qDebug() << "Finished.";

//    samples.removeFirst();

//    QStringList completeGeneIDs;

//    for (FeatureCollection collection : samples) {
//        for (Feature feature : collection.getFeatures()) {
//            completeGeneIDs.append(feature.ID);
//        }
//    }

//    completeGeneIDs.removeDuplicates();

    QString tsneFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/5k_Pbmc/merged_tsne_clusters_by_barcode.csv";

    QVector<std::tuple<QString, int, double, double>> tsneProjectionData = CSVReader::readTSNECoordinatesFromProjectionFile(tsneFilePath);

    std::sort(tsneProjectionData.begin(), tsneProjectionData.end(), []
                   (const std::tuple<QString, int, double, double> valueA, const std::tuple<QString, int, double, double> valueB) {
        return std::get<1>(valueA) < std::get<1>(valueB);
    });

    QChart * chart = new QChart();
    QVector<QScatterSeries *> allScatterSeries;

    double minObservedXValue = std::get<2>(tsneProjectionData.first()),
           maxObservedXValue = std::get<2>(tsneProjectionData.first()),
           minObservedYValue = std::get<3>(tsneProjectionData.first()),
           maxObservedYValue = std::get<3>(tsneProjectionData.first());

    QVector<int> clusterIndices;

    for (auto const & value : tsneProjectionData) {
        int newClusterIndex = std::get<1>(value);

        // If this is the first round or there is a new index,
        // append a new scatterseries and save the cluster's index
        if (clusterIndices.isEmpty() || newClusterIndex != clusterIndices.last()) {
            clusterIndices.append(newClusterIndex);
            allScatterSeries.append(new QScatterSeries());
        }

        double newXValue = std::get<2>(value),
               newYValue = std::get<3>(value);

        if (newXValue < minObservedXValue)
            minObservedXValue = newXValue;
        else if (newXValue > maxObservedXValue)
            maxObservedXValue = newXValue;

        if (newYValue < minObservedYValue)
            minObservedYValue = newYValue;
        else if (newYValue > maxObservedYValue)
            maxObservedYValue = newYValue;

        allScatterSeries.last()->append(std::get<2>(value), std::get<3>(value));
    }

    for (int i = 0; i < allScatterSeries.length(); i++) {
        QScatterSeries * series = allScatterSeries.at(i);
        chart->addSeries(series);
        series->setName("Cluster " + QString::number(clusterIndices.at(i)));
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(8);
    }

    chart->setTitle("Chart test");
    chart->setDropShadowEnabled(false);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QValueAxis * xAxis = new QValueAxis();
    QValueAxis * yAxis = new QValueAxis();
    xAxis->setTitleText("UMAP 1");
    yAxis->setTitleText("UMAP 2");
    xAxis->setRange(minObservedXValue - 1, maxObservedXValue + 1);
    yAxis->setRange(minObservedYValue - 1, maxObservedYValue + 1);

    // Add axes to the chart
    for (auto const & series : chart->series()) {
        chart->setAxisX(xAxis, series);
        chart->setAxisY(yAxis, series);
    }

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(800, 700);
    window.show();




# else
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
    QObject::connect(&coordinator, &Coordinator::finishedFileParsing, &mainWindow, &MainWindow::on_filesParsed);
    QObject::connect(&coordinator, &Coordinator::finishedCorrelating, &mainWindow, &MainWindow::on_correlatingFinished);
    QObject::connect(&coordinator, &Coordinator::sendGeneExpressionData, &mainWindow, &MainWindow::transmitGeneExpressionDataForAnalysisTab);

    // Main Window -> Coordinator
    QObject::connect(&mainWindow, &MainWindow::runAnalysis, &coordinator, &Coordinator::on_runAnalysis);
    QObject::connect(&mainWindow, &MainWindow::requestGeneExpressionDataForAnalysisTab, &coordinator, &Coordinator::on_geneExpressionDataRequested);

    // At this point, the complete control over the system workflow is handed over to the Coordinator

#endif
    return application.exec();
}
