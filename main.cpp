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

#define run 0

#if !run
#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#endif
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

#if !run
//    QString path = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/Bachelorarbeit/Programming/Data/Cell Marker/Human_cell_markers.csv";
//    QHash <QString, QVector<QPair<QString, QString>>> markerTypeHashes = CSVReader::sortCsvByMarker(path);

//    for (QString key : markerTypeHashes.keys()) {
//        qDebug() << "\n##################" << key.toUpper() << "##################\n";
//        QVector<QPair<QString, QString>> valuesForKey = markerTypeHashes[key];
//        for (QPair<QString, QString> value : valuesForKey) {
//            qDebug() << value.first << " - " << value.second;
//        }
//    }

//    QString path = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/Bachelorarbeit/Programming/Data/Pbmc_expression.csv";
//    QVector<FeatureCollection> clustersWithSignificantFeatureFoldChanges = CSVReader::getClusterFeatureExpressionFoldChanges(path, 40);

//    for (FeatureCollection cluster : clustersWithSignificantFeatureFoldChanges) {
//        qDebug() << "########################" << cluster.ID << "########################";
//        for (Feature feature : cluster.getFeatures()) {
//            qDebug() << "Feature:" << feature.ID << "- fold change:" << feature.count;
//        }
//    }

//    QString clusterFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/Bachelorarbeit/Programming/Data/Pbmc_expression.csv";
//    QVector<FeatureCollection> clustersWithSignificantFeatureFoldChanges = CSVReader::getClusterFeatureExpressionFoldChanges(path, 40);

    QString markerFilePath = "/home/kademuni/Documents/Programming/PanglaoDB_markers_07_Feb_2020.csv";
//    QString datasetFilepath = "/home/kademuni/Documents/Programming/Pbmc_expression.csv";
//    QString datasetFilepath = "/home/kademuni/Documents/Programming/Neuron_expression.csv";
    QString datasetFilepath = "/home/kademuni/Documents/Programming/5k_neuron_v3_analysis/analysis/diffexp/kmeans_10_clusters/differential_expression.csv";

    QVector<FeatureCollection> clustersWithMarkers = CSVReader::getClusterFeatures(datasetFilepath, 15, 0);

    QVector<FeatureCollection> cellTypesWithMarkers = CSVReader::getUIAndSensitivityAndSpecicifityForMarkers(markerFilePath);

    QVector<QVector<QPair<QString, double>>> bla = ExpressionComparator::findMostLikelyCellTypes(clustersWithMarkers, cellTypesWithMarkers, qMakePair(0.3, 0.8), qMakePair(0.1, 0.2), 0.3);

#endif

#if run
    // Declaration of the used widgets
    MainWindow mainWindow;
    StartDialog startDialog;
    TabWidget tabWidget;

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
