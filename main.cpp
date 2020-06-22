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
#include "Utils/Helper.h"

#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"

#define run 0

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

#if !run

    QString samplesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/Pbmc_expression.csv";
    QString cellTypesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/PanglaoDB.csv";

    qDebug() << "Parsing.";
    QVector<FeatureCollection> samples = CSVReader::read10xGenomicsClustersFromFile(samplesFilePath, {12, 0});
    QVector<FeatureCollection> cellTypes = CSVReader::readCellTypesFromPanglaoDBFile(cellTypesFilePath, {});
    qDebug() << "Finished.";

    samples.removeFirst();

    qDebug() << "Correlating.";
    QVector<QVector<QPair<QString, double>>> correlations = ExpressionComparator::findClusterCellFoldChangeCorrelations(samples, cellTypes);
    qDebug() << "Finished.";

    for (int i = 0; i < correlations.length(); i++) {
        qDebug() << "\n" << samples[i].ID << ":";
        for (int j = 0; j < 5; j++) {
            qDebug() << correlations[i][j].first << ":" << correlations[i][j].second;
        }
    }

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
