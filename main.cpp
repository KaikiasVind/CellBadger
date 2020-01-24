#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QObject>

#include "Mainwindow.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/CSVReader.h"
#include "Utils/Sorter.h"
#include "BioModels/FeatureCollection.h"
#include "Statistics/Expressioncomparator.h"
#include "Statistics/Correlator.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "System/Coordinator.h"
#include "System/InformationCenter.h"
#include "StartDialog.h"

using namespace ConfigFileOperator;
using namespace CSVReader;
using namespace ExpressionComparator;
using namespace Sorter;

#define run 1
#define old_run 0
#define testing !run
#define gui 1
#define verbose 0

#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MainWindow mainWindow;
    StartDialog startDialog;
#if gui
    startDialog.show();
#endif

#if testing
//#include "Test.h"
//    Test test;

//    auto bla = []() { qDebug() << "GEWITTER?!"; };
//    // Set up watcher that signals when parsing has been finished
////    QFutureWatcher<QVector<FeatureCollection>> futureWatcher;
//    QFutureWatcher<void> futureWatcher;
//    // Connect the watcher with the connecter slot -> The watcher will report back when the parsing has finished
//    QObject::connect(&futureWatcher, SIGNAL(finished()), &test, SLOT(on_finished()));

//    // Parse the cluster file with default cutoff in another thread
////    QFuture<QVector<FeatureCollection>> futureCellMarkersForTypes = QtConcurrent::run(CSVReader::getTissuesWithGeneExpression, datasetFilePath, 15);
//    QFuture<void> future = QtConcurrent::run(bla);
//    // Let the watcher watch over the new process
//    futureWatcher.setFuture(future);

//    // Wait for finished to avoid loosing scope before parsing has finished
//    futureWatcher.waitForFinished();
#endif

#if run
    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++
    QString configFilePath = QDir::homePath().append("/.badger.conf");
    QString defaultMarkerFilePath = QDir::homePath().append("/Desktop/gtex_analysis.csv");
    ConfigFile configFile;

    bool isConfigFileExits = ConfigFileOperator::isConfigFileExists(configFilePath);

    if (isConfigFileExits) {
        qDebug() << "Found config file:" << configFilePath;
        configFile = ConfigFileOperator::readConfigFile(configFilePath);
    } else {
        qDebug() << "No config file found.";
        ConfigFileOperator::createConfigFile(configFilePath);
        configFile = ConfigFileOperator::initializeConfigFile();
        //FIXME: THIS HAS TO BE DONE ELSEWHERE
        configFile.cellMarkersFilePath = defaultMarkerFilePath;
    }

    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++

    // ++++++++++++++++++++++++ CREATE PROGRAM BASICS +++++++++++++++++++++++++++++
    // InformationCenter is used to store all relevant software data
    InformationCenter informationCenter(configFile);
    // Coordinator is used to control the relevant data flow
    Coordinator coordinator(informationCenter);
    // ++++++++++++++++++++++++ CREATE PROGRAM BASICS +++++++++++++++++++++++++++++

    // +++++++++++++++++++++++++++++++ BUILD SIGNAL AND SLOT LOGIC +++++++++++++++++++++++++++++++
    // StartDialog -> Coordinator
    QObject::connect(&startDialog, &StartDialog::runNewProject, &coordinator, &Coordinator::on_newProjectStarted);

    // StartDialog -> MainWindow
    QObject::connect(&startDialog, &StartDialog::runNewProject, &mainWindow, &MainWindow::on_newProjectStarted);

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedFileParsing, &mainWindow, &MainWindow::on_clusterFileParsed);

    // Main Window -> Coordinator

    // +++++++++++++++++++++++++++++++ BUILD SIGNAL AND SLOT LOGIC +++++++++++++++++++++++++++++++

    // At this point, the complete control over the system workflow is handed over to the Coordinator
#endif

#if old_run
    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++
//    QString configFilePath = QDir::homePath().append("/.badger.conf");
//    ConfigFileOperator configFileOperator;

//    bool isConfigFileExits = configFileOperator.isConfigFileExists(configFilePath);

//    QString cellMarkersFilePath,
//            clusterExpressionFilePath;

//    if (isConfigFileExits) {
//        qDebug() << "Reading config file:" << configFilePath;
//        configFileOperator.readConfigFile(configFilePath);
//        cellMarkersFilePath = configFileOperator.getCellMarkersFilePath();
//        clusterExpressionFilePath = configFileOperator.getClusterExpressionFilePath();
//    } else {
//        qDebug() << "No config file found under:" << configFilePath;
//        configFileOperator.createConfigFile(configFilePath);
//    InformationCenter
    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++

    qDebug() << "Reading 10x cluster expression file.";
    QVector<FeatureCollection> xClusterGeneExpressions = CSVReader::getClusterFeatureExpressions(clusterExpressionFilePath, 15);
    qDebug() << "Done";

    qDebug() << "Reading marker expression file.";
    QVector<FeatureCollection> tissues = CSVReader::getTissuesWithGeneExpression(cellMarkersFilePath, 100);
    qDebug() << "Done";

    qDebug() << xClusterGeneExpressions[0].getNumberOfFeatures() << ":" << tissues[0].getNumberOfFeatures();

    qDebug() << "Finding cluster - tissue correlations";
    QVector<QVector<QPair<QString, double>>> clustersWithTissueCorrelations;
    clustersWithTissueCorrelations.reserve(xClusterGeneExpressions.length());
    clustersWithTissueCorrelations = ExpressionComparator::findClusterTissueCorrelations(xClusterGeneExpressions, tissues);
    qDebug() << "Done";

#if verbose
    int i = 0;
    for (QVector<QPair<QString, double>> clusterWithTissueCorrelations : clustersWithTissueCorrelations) {
        qDebug() << "cluster:" << i++;
        for (QPair<QString, double> correlation : clusterWithTissueCorrelations) {
            qDebug() << correlation.first << ":" << correlation.second;
        }
        qDebug() << "\n";
    }
#endif
#endif

#if gui
//    w.plotHeatMap(clustersWithTissueCorrelations);
#endif


//    for (QPair<QString, double> pair : clusterTissueCorrelations) {
//        qDebug() << pair.first << ":" << pair.second;
//    }
//    qDebug() << "\n";

//    qDebug() << "Finding equally expressed Features";
//    QVector<QPair<Feature, Feature>> equallyExpressedFeatures = Sorter::findEquallyExpressedFeatures(tissues[0], xClusterGeneExpressions[0]);

//    qDebug() << "Equally expressed features:";
//    for (int i = 0; i < equallyExpressedFeatures.length(); i++) {
//        qDebug() << equallyExpressedFeatures[i].first.ID << ":" << equallyExpressedFeatures[i].first.count << "-" <<
//                    equallyExpressedFeatures[i].second.ID << ":" << equallyExpressedFeatures[i].second.count;
//    }

//    QVector<double> tissueFeatureExpressions = tissues[0].getMostExpressedFeaturesCounts(200);
//    QVector<double> clusterFeatureExpressions = xClusterGeneExpressions[0].getMostExpressedFeaturesCounts(200);

//    qDebug() << tissueFeatureExpressions << "\n" << clusterFeatureExpressions;
//    qDebug() << "Result:" << Correlator::calculateSpearmanCorrelation(tissueFeatureExpressions, clusterFeatureExpressions);

//    QVector<FeatureCollection> rankedClusterGeneExpressions;
//    QVector<FeatureCollection> rankedTissueGeneExpressions;
//    qDebug() << "Ranking cluster features";
//    for (FeatureCollection collection : xClusterGeneExpressions)
//        rankedClusterGeneExpressions.append(Sorter::sortFeaturesByExpression(collection));
//    qDebug() << "Done";
//    qDebug() << "Ranking tissue features";
//    for (FeatureCollection collection : tissues)
//        rankedTissueGeneExpressions.append(Sorter::sortFeaturesByExpression(collection));
//    qDebug() << "Done";

//    QVector<double> variableOne = {59, 35, 43, 23, 42, 27},
//                    variableTwo = {14.61, 11.80, 14.34, 13.03, 14.18, 11.02};


    return application.exec();
}
