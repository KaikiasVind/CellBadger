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


// ###################################### INTERACTION WITH MAIN WINDOW ###########################################

/**
 * @brief Coordinator::on_newProjectStarted - Starts and parses each uploaded dataset in a different thread
 * @param datasetFilePaths - List of file-paths that have been uploaded
 */
void Coordinator::on_newProjectStarted(QStringList datasetFilePaths) {

    for (QString datasetFilePath : datasetFilePaths) {
        // Parse the cluster file with default cutoff in another thread
        QFuture<QVector<FeatureCollection>> futureCellMarkersForTypes = QtConcurrent::run(CSVReader::getClusterFeatureExpressions, datasetFilePath, 15);

        // Let the multi-thread-watcher watch over the new process
        parsingThreadsWatcher.addFuture(futureCellMarkersForTypes);
    }

    // Wait for finished to avoid loosing scope before parsing has finished
    parsingThreadsWatcher.waitForFinished();

    // Gather the information that was parsed from the different threads
    gatherInformationAfterParsingFinished();
}

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

// ######################################### FILE PROCESSING ##################################################
void Coordinator::gatherInformationAfterParsingFinished() {
    qDebug() << "Finished parsing.";

    // Gather the results from every finished thread
    for (QFuture<QVector<FeatureCollection>> future : parsingThreadsWatcher.futures()) {
        // put it together
        QVector<FeatureCollection> parsedCollections;
        parsedCollections.append(future.result());

        // and report it the the information center
        informationCenter.xClusterCollections.append(parsedCollections);
    }

    // Report that the last parsing thread has finished to the main window
    emit finishedFileParsing();
}
