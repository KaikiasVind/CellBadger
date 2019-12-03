#include "Coordinator.h"

#include <QObject>
#include <QStringList>
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

void Coordinator::on_programStarted(QStringList filePaths) {
    qDebug() << "on_filesUploaded: received" << filePaths;
    // Parse the cluster file with default cutoff
    informationCenter.cellMarkersForTypes = CSVReader::getTissuesWithGeneExpression(filePaths[0], 15);

    // and report back to the main window when parsing has finished
    emit finishedCellMarkerFileParsing();
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
    emit finishedClusterFileParsing();
}


/**
 * @brief Coordinator::on_projectFileUploaded
 * @param filePath
 */
void Coordinator::on_projectFileUploaded(QStringList filePath) {
    qDebug() << "on_projectFileUploaded: received" << filePath;
}
// ################### INTERACTION WITH MAIN WINDOW ########################
