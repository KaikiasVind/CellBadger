#include "Coordinator.h"

#include <QObject>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QDebug>

#include "System/InformationCenter.h"
#include "Utils/FileOperators/CSVReader.h"

/**
 * @brief Coordinator::Coordinator
 */
Coordinator::Coordinator(InformationCenter informationCenter)
    : informationCenter {informationCenter}
{}


/**
 * @brief Coordinator::gatherInformationAfterParsingFinished - Gathers the parsed information from the opened threads and reports it to the information center
 */
void Coordinator::gatherInformationAfterParsingFinished() {
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

    for (FeatureCollection collection : informationCenter.cellMarkersForTypes) {
        qDebug() << "ID:" << collection.ID << "# features:" << collection.getNumberOfFeatures();
    }

    // Report that the last parsing thread has finished to the main window
    emit finishedFileParsing();
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

    // Parse the cell marker file with default cutoff in a new thread
    QFuture<QVector<FeatureCollection>> futureCellMarkersForTypes = QtConcurrent::run(CSVReader::getTissuesWithGeneExpression, cellMarkerFilePath, 100);

    // Let the multi-thread-watcher watch over the new process -> The first thread has been reserved for the marker file
    parsingThreadsWatcher.addFuture(futureCellMarkersForTypes );

    // Do the same for all datasets that were uploaded
    for (QString datasetFilePath : datasetFilePaths) {
        // Parse the cluster file with default cutoff in a new thread
        QFuture<QVector<FeatureCollection>> futureClusters = QtConcurrent::run(CSVReader::getClusterFeatureExpressions, datasetFilePath, 15);

        // Let the multi-thread-watcher watch over the new process
        parsingThreadsWatcher.addFuture(futureClusters);
    }

    // Wait for finished to avoid loosing scope before parsing has finished
    parsingThreadsWatcher.waitForFinished();

    // Gather the information that was parsed from the different threads
    gatherInformationAfterParsingFinished();
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
