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

#define gui 1

int main(int argc, char * argv[])
{
    QApplication application(argc, argv);

#if !gui

//    QString samplesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/Pbmc_expression.csv";
//    QString cellTypesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/PanglaoDB_markers.csv";

    QString samplesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\10xGenomics\\Pbmc_expression.csv";
    QString cellTypesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\10xGenomics\\PanglaoDB_markers.csv";

    qDebug() << "Parsing.";
    QVector<FeatureCollection> samples = CSVReader::read10xGenomicsClustersFromFile(samplesFilePath, {15, 0});
    QVector<FeatureCollection> cellTypes = CSVReader::readCellTypesFromPanglaoDBFile(cellTypesFilePath, {});
    qDebug() << "Finished.";

    samples.removeFirst();

    QStringList completeGeneIDs;

    for (FeatureCollection collection : samples) {
        for (Feature feature : collection.getFeatures()) {
            completeGeneIDs.append(feature.ID);
        }
    }

    completeGeneIDs.removeDuplicates();

    AnalysisConfigModel analysisConfigModel(Definitions::AnalysisFilterMode::MANUAL);

    analysisConfigModel.minRawCount = 20;
    analysisConfigModel.maxRawCount = 972;
//    analysisConfigModel.includeRawCountInAtLeast = true;
//    analysisConfigModel.rawCountInAtLeast = 3;
    analysisConfigModel.minFoldChange = 0;
    analysisConfigModel.maxFoldChange = 906;

    QVector<FeatureCollection> filteredSamples = Helper::filterExpressedGenesAccordingToFilters(samples, completeGeneIDs, analysisConfigModel);
////    QVector<FeatureCollection> filteredSamples = Helper::findTopNMostExpressedGenes(samples, 50);

    QVector<QVector<QPair<QString, double>>> correlations = ExpressionComparator::findClusterCellFoldChangeCorrelations(samples, cellTypes);
    QVector<QVector<QPair<QString, double>>> filteredCorrelations = ExpressionComparator::findClusterCellFoldChangeCorrelations(filteredSamples, cellTypes);

    QVector<double> qualityScores = Math::calculateQualityScores(filteredCorrelations);

    auto printCorrelations = [qualityScores](QVector<QVector<QPair<QString, double>>> correlationOneValues, QVector<QVector<QPair<QString, double>>> correlationTwoValues) {
        for (int i = 0; i < correlationOneValues.length(); i++) {
            qDebug() << "\ncluster" << i << "- qs:" << qualityScores.at(i);
            for (int j = 0; j < 5; j++) {
//                qDebug() << correlationOneValues.at(i).at(j).first << "-" << correlationOneValues.at(i).at(j).second;
                qDebug() << correlationTwoValues.at(i).at(j).first << "-" << correlationTwoValues.at(i).at(j).second;
            }
        }
    };

    for (int i = 0; i < correlations.length(); i++) {
        for (int j = 0; j < correlations.at(i).length(); j++) {
            bool equalGeneIDs = correlations.at(i).at(j).first.compare(filteredCorrelations.at(i).at(j).first) == 0,
                 equalValues  = correlations.at(i).at(j).second == filteredCorrelations.at(i).at(j).second;

            if (!equalGeneIDs || !equalValues) {
                qDebug() << "Not identical!";
                printCorrelations(correlations, filteredCorrelations);
                exit(0);
            }
        }
    }

    qDebug() << "Identical!";
    printCorrelations(correlations, filteredCorrelations);
    exit(0);



//    Feature a("a", "nAn", 15, -1, 15);
//    Feature b("b", "nAn", 12, -1, 13);
//    Feature c("c", "nAn", 20, -1, 10);

//    Feature d("d", "nAn", 300, -1, 400);
//    Feature e("e", "nAn", 9, -1, 21);
//    Feature f("f", "nAn", -40, -1, -30);

//    FeatureCollection testCollection("test");
//    testCollection.addFeature(a);
//    testCollection.addFeature(b);
//    testCollection.addFeature(c);
//    testCollection.addFeature(d);
//    testCollection.addFeature(e);
//    testCollection.addFeature(f);

//    QVector<FeatureCollection> experimentOne, experimentTwo;
//    experimentOne << testCollection;
//    experimentTwo << testCollection;
//    QVector<QVector<FeatureCollection>> experiments;
//    experiments << experimentOne << experimentTwo;

//    AnalysisConfigModel configModel(Definitions::AnalysisFilterMode::MANUAL);
//    configModel.minRawCount = 10;
//    configModel.maxRawCount = 20;
//    configModel.minFoldChange = 10;
//    configModel.maxFoldChange = 20;

//    QVector<QVector<FeatureCollection>> filteredExperiments;

//    for (QVector<FeatureCollection> experiment : experiments)
//        filteredExperiments.append(Helper::filterExpressedGenesAccordingToFilters(experiment, {"a", "b", "c", "d", "e", "f"}, configModel));

//    for (QVector<FeatureCollection> experiment : filteredExperiments) {
//        for (FeatureCollection collection : experiment) {
//            qDebug() << collection.ID << ":";
//            for (Feature feature : collection.getFeatures()) {
//                qDebug() << feature.ID;
//            }
//        }
//        qDebug() << "\n";
//    }




//    QVector<std::tuple<QString, QVector<double>, double>> genesWithRawCountsInClusters = Helper::getFeatureCollectionsAsGenes(samples, allGeneIDs);

//    qDebug() << "length:" << genesWithRawCountsInClusters.length();
//    for (int i = 0; i < genesWithRawCountsInClusters.length(); i++) {
//        qDebug() << "\nGene:" << std::get<0>(genesWithRawCountsInClusters.at(i));
//        qDebug() << std::get<1>(genesWithRawCountsInClusters.at(i));
//        qDebug() << "mean:" << std::get<2>(genesWithRawCountsInClusters.at(i));
//    }

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

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedCorrelating, &mainWindow, &MainWindow::on_correlatingFinished);

    // Main Window -> Coordinator
    QObject::connect(&mainWindow, &MainWindow::runAnalysis, &coordinator, &Coordinator::on_runAnalysis);

    // At this point, the complete control over the system workflow is handed over to the Coordinator

#endif
    return application.exec();
}
