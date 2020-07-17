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


#define run 1

int main(int argc, char * argv[])
{
    QApplication application(argc, argv);

#if !run

    QString samplesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/Pbmc_expression.csv";
    QString cellTypesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/PanglaoDB_markers.csv";

//    QString samplesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\Pbmc_expression.csv";
//    QString cellTypesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\PanglaoDB_markers.csv";

    qDebug() << "Parsing.";
    QVector<FeatureCollection> samples = CSVReader::read10xGenomicsClustersFromFile(samplesFilePath, {15, 0});
    QVector<FeatureCollection> cellTypes = CSVReader::readCellTypesFromPanglaoDBFile(cellTypesFilePath, {});
    qDebug() << "Finished.";

    samples.removeFirst();

//    for (FeatureCollection collection : samples) {
//        qDebug() << "\n" << collection.ID;
//        for (int i = 0; i < 16; i++) {
//            Feature feature = collection.getFeature(i);
//            qDebug() << feature.ID << "-" << feature.count;
//        }
//    }

    QVector<QVector<QPair<QString, double>>> correlations = ExpressionComparator::findClusterCellFoldChangeCorrelations(samples, cellTypes);

    QVector<double> qualityScores = Math::calculateQualityScores(correlations);

    for (int i = 0; i < correlations.length(); i++) {
        qDebug() << "\ncluster" << i << "- qs:" << qualityScores.at(i);
        for (int j = 0; j < 5; j++) {
            qDebug() << correlations.at(i).at(j).first << "-" << correlations.at(i).at(j).second;
        }
    }



//    QVector<std::tuple<QString, QVector<double>, double>> genesWithRawCountsInClusters = Helper::getFeatureCollectionsAsGenes(samples, allGeneIDs);

//    qDebug() << "length:" << genesWithRawCountsInClusters.length();
//    for (int i = 0; i < genesWithRawCountsInClusters.length(); i++) {
//        qDebug() << "\nGene:" << std::get<0>(genesWithRawCountsInClusters.at(i));
//        qDebug() << std::get<1>(genesWithRawCountsInClusters.at(i));
//        qDebug() << "mean:" << std::get<2>(genesWithRawCountsInClusters.at(i));
//    }

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
    QObject::connect(&coordinator, &Coordinator::finishedFileParsing, &mainWindow, &MainWindow::on_filesParsed);

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedCorrelating, &mainWindow, &MainWindow::on_correlatingFinished);

    // Main Window -> Coordinator
    QObject::connect(&mainWindow, &MainWindow::runAnalysis, &coordinator, &Coordinator::on_runAnalysis);

    // At this point, the complete control over the system workflow is handed over to the Coordinator

#endif
    return application.exec();
}
