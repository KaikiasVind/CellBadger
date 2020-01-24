#include "Coordinator.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QDebug>

#include <iostream>
using std::cout;
using std::endl;

#include "System/InformationCenter.h"
#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"

/**
 * @brief Coordinator::Coordinator
 */
Coordinator::Coordinator(InformationCenter informationCenter)
    : informationCenter {informationCenter}
{}


template<typename F>
/**
 * @brief Coordinator::parseFile - Parse the given dataset files with the corresponding function in separate threads and hand them over to the thread watcher
 * @param filePaths - List of file paths corresponding to the dataset / marker files
 * @param parsingFunction - The function with which the given file(s) can be parsed properly
 * @param cutoff - The cutoff that should be used for parsing
 */
void Coordinator::parseFiles(const QStringList filePaths, const F & parsingFunction, const double cutoff) {
    for (QString filePath: filePaths) {
        // Parse the file with given cutoff in a new thread with given function
        QFuture<QVector<FeatureCollection>> futureParsedFile = QtConcurrent::run(parsingFunction, filePath, cutoff);

        // And let the multi-thread-watcher watch over the new process
        parsingThreadsWatcher.addFuture(futureParsedFile);
    }
}


/**
 * @brief Coordinator::gatherInformationAfterParsingFinished - Gathers the parsed information from the opened threads and reports it to the information center
 */
void Coordinator::saveInformationAfterParsingFinished() {
    qDebug() << "Finished parsing.";

    // The first thread has been reserved for the marker file -> Report the result to the information center
    informationCenter.cellMarkersForTypes = parsingThreadsWatcher.futures()[0].result();

    // Gather the results from the other finished threads,
    for (int i = 1; i < parsingThreadsWatcher.futures().length(); i++) {
        // put it together
//        QVector<FeatureCollection> parsedCollections;
//        parsedCollections.append(parsingThreadsWatcher.futures()[i].result());

        // and report it the the information center
        informationCenter.xClusterCollections.append(parsingThreadsWatcher.futures()[i].result());
    }
}


/**
 * @brief Coordinator::correlateDatasets - Correlates the given datasets with the given set of cell markers in separate threads and hands them over to the thread watcher
 * @param xClusterDatasets - List of cluster datasets that are to be correlated
 * @param cellMarkersForTypes - Cell type cell markers that are used to correlate against
 */
void Coordinator::correlateDatasets(const QVector<QVector<FeatureCollection>> xClusterDatasets, const QVector<FeatureCollection> cellMarkersForTypes) {
    for (QVector<FeatureCollection> xClusterDataset : xClusterDatasets) {
        // Correlate the single dataset with the given set of cell type markers
        QFuture<QVector<QVector<QPair<QString, double>>>> futureCorrelations = QtConcurrent::run(ExpressionComparator::findClusterTissueCorrelations, xClusterDataset, cellMarkersForTypes);

        // And let the corresponding multi-thread-watcher watch over the new process
        this->correlatorThreadsWatcher.addFuture(futureCorrelations);
    }
}


/**
 * @brief Coordinator::saveInformationAfterCorrelatingFinished - Gathers the information from the finished correlation from the opened threads and reports it to the information center
 */
void Coordinator::saveInformationAfterCorrelatingFinished() {
    // Gather the results from every finished thread
    for (int i = 0; i < correlatorThreadsWatcher.futures().length(); i++) {
        // and report it to the information center
        this->informationCenter.correlatedDatasets.append(correlatorThreadsWatcher.futures()[i].result());
    }
}

void Coordinator::printResults() {
    int i = 0;
    int j = 0;
    for (QVector<QVector<QPair<QString, double>>> correlatedDataset : informationCenter.correlatedDatasets) {
        qDebug() << "\n########################### DATASET:" << i++ << "###########################\n";
        j = 0;
        for (QVector<QPair<QString, double>> clusterWithTissueCorrelations : correlatedDataset) {
            qDebug() << "\nCLUSTER:" << j++ << "\n";
            for (QPair<QString, double> correlation : clusterWithTissueCorrelations) {
                qDebug() << correlation.first << ":" << correlation.second;
            }
        }
    }
}

// ###################################### INTERACTION WITH START DIALOG ###########################################

/**
 * @brief Coordinator::on_newProjectStarted - Starts and parses the marker file and  each uploaded dataset in a different thread
 * @param datasetFilePaths - List of file-paths that have been uploaded
 * @param cellMarkerFilePath - One of: File path to cell marker file OR "nAn" if none was given
 */
void Coordinator::on_newProjectStarted(QString cellMarkerFilePath, QStringList datasetFilePaths) {

    // If no cell marker file was uploaded, use the default one
    if (cellMarkerFilePath == "nAn") { //REMEMBER: Is that really a nice thing to do?
        cellMarkerFilePath = informationCenter.configFile.cellMarkersFilePath;
    }

    // REMEMBER: Find another way to do this -> Another typename and type deduction?
    QStringList cellMarkerFilePaths;
    cellMarkerFilePaths.reserve(1);
    cellMarkerFilePaths.append(cellMarkerFilePath);

    qDebug() << "Parsing:" << cellMarkerFilePaths.first();
    // Parse the cell marker file in separate thread
    cout << "Parsing cell marker file." << endl;
    this->parseFiles(cellMarkerFilePaths, CSVReader::getTissuesWithGeneExpression, 100);

    // Parse the dataset files in separate threads
    cout << "Parsing datasets." << endl;
    this->parseFiles(datasetFilePaths, CSVReader::getClusterFeatureExpressions, 15);

    // Wait for finished to avoid loosing scope before parsing has finished
    this->parsingThreadsWatcher.waitForFinished();
    cout << "Finished parsing. Gathering information" << endl;

    // Gather and save the information that was parsed from the different threads
    this->saveInformationAfterParsingFinished();
    cout << "Saving information successfull." << endl;

    // Report that the last parsing thread has finished to the main window
    emit finishedFileParsing();

    cout << "Correlating datasets" << endl;
    // Correlate the datasets with the given cell type markers in separate threads
    this->correlateDatasets(informationCenter.xClusterCollections, informationCenter.cellMarkersForTypes);
    cout << "Finished correlating. Gathering information" << endl;

    // Gather and save the information from the correlation from the different threads
    this->saveInformationAfterCorrelatingFinished();
    cout << "Saving correlation data successfull." << endl;

    // Report that the last correlation thread has finished to the main window
//    emit finishedCorrelating(informationCenter.correlatedDatasets);
    emit finishedCorrelating(this->informationCenter);

    qDebug() << "Finished workflow. YEAY." << endl;
}


// ###################################### INTERACTION WITH MAIN WINDOW ###########################################
/**
 * @brief Coordinator::on_filesUploaded
 * @param filePaths
 */
void Coordinator::on_filesUploaded(QStringList filePaths) {
    qDebug() << "on_filesUploaded: received" << filePaths;
    //REMEMBER: How to handle the cutoff here -> Should the user be able to change the value beforehand?
    // Parse the cluster file with default cutoff

    // and report back to the main window when parsing has finished
//    emit finishedClusterFileParsing();
}


/**
 * @brief Coordinator::on_projectFileUploaded
 * @param filePath
 */
void Coordinator::on_projectFileUploaded(QStringList filePath) {
    qDebug() << "on_projectFileUploaded: received" << filePath;
}
