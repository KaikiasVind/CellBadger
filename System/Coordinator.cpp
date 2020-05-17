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


/**
 * @brief parseClusterFiles
 * @param filePaths
 * @param meanCountCutOff
 * @param foldChangeCutOff
 */
void Coordinator::parseClusterFiles(const QStringList filePaths, const double meanCountCutOff, const double foldChangeCutOff) {
    for (QString filePath: filePaths) {
        // Parse the file with given cutoff in a new thread with given function
        QFuture<QVector<FeatureCollection>> futureParsedFile = QtConcurrent::run(CSVReader::read10xGenomicsClustersFromFile, filePath, meanCountCutOff, foldChangeCutOff);

        // And let the multi-thread-watcher watch over the new process
        parsingThreadsWatcher.addFuture(futureParsedFile);
    }
}


/**
 * @brief Coordinator::parseMarkerFile
 * @param filePaths
 * @param expressionCountCutOff
 */
void Coordinator::parseMarkerFile(const QString filePath, const double expressionCountCutOff) {
    qDebug() << "FUNKTION FEHLT!!!";
    exit(1);
    // Parse the file with given cutoff in a new thread with given function
//	QFuture<QVector<FeatureCollection>> futureParsedFile = QtConcurrent::run(CSVReader::, filePath, meanCountCutOff, foldChangeCutOff);

    // And let the multi-thread-watcher watch over the new process
//    parsingThreadsWatcher.addFuture(futureParsedFile);
}


/**
 * @brief Coordinator::gatherInformationAfterParsingFinished - Gathers the parsed information from the opened threads and reports it to the information center
 */
void Coordinator::saveInformationAfterParsingFinished() {
    qDebug() << "Finished parsing.";

    // The first thread has been reserved for the marker file -> Report the result to the information center
    this->informationCenter.cellMarkersForTypes = parsingThreadsWatcher.futures()[0].result();

    // Get the first marker-FeatureCollection that is only used to store the IDs for genes expressed by at least one cluster
    FeatureCollection completeSetOfGeneIDs = this->informationCenter.cellMarkersForTypes.first();
    this->informationCenter.completeSetOfGeneIDs.reserve(completeSetOfGeneIDs.getNumberOfFeatures());

    // Gather all gene IDs from the FeatureCollection and report them to the information center
    for (Feature feature : completeSetOfGeneIDs.getFeatures()) {
        this->informationCenter.completeSetOfGeneIDs.append(feature.ID);
    }

    // Removing of the marker-FeatureCollection leaves only the "real" FeatureCollections parsed from the files
    this->informationCenter.cellMarkersForTypes.removeFirst();

    // Gather the results from the other finished threads,
    for (int i = 1; i < this->parsingThreadsWatcher.futures().length(); i++) {

        // and report it the the information center
        this->informationCenter.xClusterCollections.append(this->parsingThreadsWatcher.futures()[i].result());
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
//        QFuture<QVector<QVector<QPair<QString, double>>>> futureCorrelations = QtConcurrent::run(ExpressionComparator::findClusterTissueCorrelations, xClusterDataset, cellMarkersForTypes);

        // And let the corresponding multi-thread-watcher watch over the new process
//        this->correlatorThreadsWatcher.addFuture(futureCorrelations);
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
        cout << "\n########################### DATASET:" << i++ << "###########################\n";
        j = 0;
        for (QVector<QPair<QString, double>> clusterWithTissueCorrelations : correlatedDataset) {
            cout << "\nCLUSTER:" << j++ << "\n";
            for (QPair<QString, double> correlation : clusterWithTissueCorrelations) {
                cout << correlation.first.toStdString() << ":" << correlation.second << endl;
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
void Coordinator::on_newProjectStarted(const QString cellMarkerFilePath, const QStringList datasetFilePaths) {
    // Add file-paths of newly uploaded datasets to file-path list
    this->informationCenter.datasetFilePaths = datasetFilePaths;

    // Working with copy fur purity
    QString markerFilePath;

    // If no cell marker file was uploaded, use the default one
    if (cellMarkerFilePath == "nAn") { //REMEMBER: Is that really a nice thing to do?
        markerFilePath = informationCenter.configFile.cellMarkersFilePath;
    }

    qDebug() << "Parsing:" << markerFilePath;
    // Parse the cell marker file in separate threads
    cout << "Parsing cell marker file." << endl;
    this->parseMarkerFile(markerFilePath, 100);

    // Parse the dataset files in separate threads
    cout << "Parsing datasets." << endl;
    this->parseClusterFiles(datasetFilePaths, 15, 1);

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
void Coordinator::on_filesUploaded(const QStringList filePaths) {
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
void Coordinator::on_projectFileUploaded(const QStringList filePath) {
    qDebug() << "on_projectFileUploaded: received" << filePath;
}
