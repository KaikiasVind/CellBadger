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


// ################### INTERACTION WITH MAIN WINDOW ########################

void Coordinator::on_newProjectStarted(QStringList datasetFilePaths) {
//    qDebug() << "on_filesUploaded: received" << datasetFilePaths;

    for (QString datasetFilePath : datasetFilePaths) {
        // Set up watcher that signals when parsing has been finished
        // Connect the watcher with the connecter slot -> The watcher will report back when the parsing has finished
        QObject::connect(&parsingThreadWatcher, SIGNAL(finished()), this, SLOT(on_datasetFileParsed()));

        // Parse the cluster file with default cutoff in another thread
        QFuture<QVector<FeatureCollection>> futureCellMarkersForTypes = QtConcurrent::run(CSVReader::getClusterFeatureExpressions, datasetFilePath, 15);

        // Let the watcher watch over the new process
        parsingThreadWatcher.setFuture(futureCellMarkersForTypes);

        // Wait for finished to avoid loosing scope before parsing has finished
        parsingThreadWatcher.waitForFinished();
    }
}

/**
 * @brief Coordinator::on_filesUploaded
 * @param filePaths
 */
void Coordinator::on_filesUploaded(QStringList filePaths) {
    qDebug() << "on_filesUploaded: received" << filePaths;
    //REMEMBER: How to handle the cutoff here -> Should the user be able to change the value beforehand?
    // Parse the cluster file with default cutoff
    informationCenter.xClusters = CSVReader::getClusterFeatureExpressions(filePaths[0], 100);

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
// ################### INTERACTION WITH MAIN WINDOW ########################

// ######################### FILE PROCESSING ################################
void Coordinator::on_datasetFileParsed() {
    qDebug() << "Finished parsing.";
    informationCenter.xClusters.append(parsingThreadWatcher.result());
}
