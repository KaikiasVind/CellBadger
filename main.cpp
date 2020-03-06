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

    QString markerFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/Bachelorarbeit/Programming/Data/PanglaoDB_markers_07_Feb_2020.tsv";
    QString datasetFilepath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/Bachelorarbeit/Programming/Data/Pbmc_expression.csv";

//    QVector<FeatureCollection> cellTypesWithMarkers = CSVReader::getUIAndSensitivityAndSpecicifityForMarkers(markerFilePath);

    QVector<FeatureCollection> clusterWithMarker = CSVReader::getClusterFeatures(datasetFilepath, 15, 5);

    for (FeatureCollection cluster : clusterWithMarker) {
        qDebug() << "\nCluster:" << cluster.ID;
        for (Feature gene : cluster.getFeatures()) {
            qDebug() << gene.ID << "-umi_count:" << gene.count << "-fold_change:" << gene.foldChange;
        }
    }

//    QVector<QVector<QPair<QString, double>>> correlations;

//    // Go through every cluster that was parsed from the 10x cluster file
//    for (int i = 0; i < clusterWithMarker.length(); i++) {
//        qDebug() << "cluster:" << clusterWithMarker.at(i).ID;

//        // And go through every cell type that was taken from the pangloDB marker file
//        for (int j = 0; j < cellTypesWithMarkers.length(); j++) {

//            // Find genes that are expresed in the cluster and are found in the cell type
//            FeatureCollection cluster = clusterWithMarker[i],
//                              cellType = cellTypesWithMarkers[j];

//            QVector<QPair<QString, double>> expressedGenesWithWeight;

//            // For every feature in the current cluster
//            for (int k = 0; k < cluster.getNumberOfFeatures(); k++) {
//                QString geneID = cluster.getFeatureID(k);

//                for (int l = 0; l < cellType.getNumberOfFeatures(); l++) {
//                    bool isSameGeneID = cellType.getFeatureID(l).compare(geneID) == 0;

//                    if (isSameGeneID) {
//                        qDebug() << "Found equal gene:" << geneID;
//    //                    double
//                    }
//                }
//            }
//        }
//        break;
//    }

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
