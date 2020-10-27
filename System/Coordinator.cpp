#include "Coordinator.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QDebug>

#include <QThread>

#include "System/InformationCenter.h"
#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#include "Utils/Math.h"
#include "Utils/Definitions.h"
#include "Utils/Helper.h"

using Definitions::AnalysisFilterMode;

/**
 * @brief Coordinator::Coordinator
 */
Coordinator::Coordinator(InformationCenter informationCenter)
    : informationCenter {informationCenter}, needsCleaning(false)
{}


template<typename F>
/**
 * @brief Coordinator::parseFiles - Parse the given dataset files with the corresponding function in separate threads and hand them over to the thread watcher
 * @param filePaths - List of file paths corresponding to the dataset / marker files
 * @param parsingFunction - The function with which the given file(s) can be parsed properly
 * @param cutoff - The cutoff that should be used for parsing
 */
void Coordinator::parseFiles(const QStringList filePaths, F & parsingFunction, const QVector<double> cutOffs) {
    for (QString filePath : filePaths) {
        // Parse the file with given cutoff in a new thread with given function
        QFuture<QVector<FeatureCollection>> futureParsedFile = QtConcurrent::run(parsingFunction, filePath, cutOffs);

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
    this->informationCenter.cellMarkersForTypes = parsingThreadsWatcher.futures()[0].result();

    // Gather the results from the other finished threads,
    for (int i = 1; i < this->parsingThreadsWatcher.futures().length(); i++) {

        QStringList completeSetOfGeneIDs;

        // and report it the the information center
        this->informationCenter.xClusterCollections.append(this->parsingThreadsWatcher.futures()[i].result());

        // The first collection in the set of collections that were parsed from the datasetfiles
        // is reserved for the genes that were expressed at least once in any cluster from the file
        for (Feature feature : this->informationCenter.xClusterCollections.last().first().getFeatures()) {
            completeSetOfGeneIDs.append(feature.ID);
        }

        // After gathering the gene IDs, remove the collection only leaving behind only the "real" FeatureCollections
        this->informationCenter.xClusterCollections.last().removeFirst();

        // Remove any duplicate gene IDs that may occur due to the gene being expressed in more that one cluster in any dataset file
        completeSetOfGeneIDs.removeDuplicates();

        // And add the set of gene IDs that belongs to this dataset to the list of all gene ID sets
        this->informationCenter.completeSetsOfGeneIDsPerDataset.append(completeSetOfGeneIDs);

        // Gather all cluster names from the gene expressions list
        QStringList clusterNames;
        std::transform(this->informationCenter.xClusterCollections.last().begin(), this->informationCenter.xClusterCollections.last().end(),
                       std::back_inserter(clusterNames), [](FeatureCollection featureCollection) { return featureCollection.ID; });

        this->informationCenter.clusterNamesForDatasets.append(clusterNames);
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
        QFuture<QVector<QVector<QPair<QString, double>>>> futureCorrelations = QtConcurrent::run(ExpressionComparator::findClusterCellFoldChangeCorrelations, xClusterDataset, cellMarkersForTypes);

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

        // Calculate the quality scores and append them to the list of previous quality scores
        this->informationCenter.qualityScores.append(Math::calculateQualityScores(this->informationCenter.correlatedDatasets.last()));
    }
    qDebug() << "Saved.";
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
    QStringList cellMarkerFilePaths;
    cellMarkerFilePaths.reserve(1);

    // If no cell marker file was uploaded, use the default one
    if (cellMarkerFilePath == "nAn") { //REMEMBER: Is that really a nice thing to do?
        cellMarkerFilePaths.append(informationCenter.configFile.cellMarkersFilePath);
    } else {
        cellMarkerFilePaths.append(cellMarkerFilePath);
    }

    qDebug() << "Parsing:" << cellMarkerFilePaths.first();
    // Parse the cell marker file in separate threads
    qDebug() << "Parsing cell marker file.";
    this->parseFiles(cellMarkerFilePaths, CSVReader::readCellTypesFromPanglaoDBFile, {});

    // Parse the dataset files in separate threads - A raw count cut-off of 1 is used to remove all underrepresented genes
    qDebug() << "Parsing datasets.";
    this->parseFiles(datasetFilePaths, CSVReader::read10xGenomicsClustersFromFile, {15, 0});

    // Wait for finished to avoid loosing scope before parsing has finished
    this->parsingThreadsWatcher.waitForFinished();
    qDebug() << "Finished parsing. Gathering information";

    // Gather and save the information that was parsed from the different threads
    this->saveInformationAfterParsingFinished();
    qDebug() << "Saving information successfull.";

    // Report that the last parsing thread has finished to the main window
    emit finishedFileParsing(this->informationCenter);

    qDebug() << "Finished workflow. YEAY.";
}


/**
 * @brief Coordinator::cleanData - Clean all data structures that have been filled by a previous analysis
 */
void Coordinator::cleanData() {
    this->parsingThreadsWatcher.clearFutures();
    this->correlatorThreadsWatcher.clearFutures();
    this->informationCenter.correlatedDatasets.clear();
    this->informationCenter.correlatedDatasets.squeeze();
    this->informationCenter.qualityScores.clear();
    this->informationCenter.qualityScores.squeeze();
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


void Coordinator::on_runAnalysis(const AnalysisConfigModel analysisConfigModel) {
    qDebug() << "Coordinator: Received signal for run analyis.";

    if (this->needsCleaning)
        this->cleanData();

    // Create a list that will serve as filtered copy of the existant experiments
    QVector<QVector<FeatureCollection>> filteredXClusterCollections;
    filteredXClusterCollections.reserve(this->informationCenter.xClusterCollections.length());

    // Go through every experiment and filter the genes according to the currently chosen filter model
    for (int i = 0; i < this->informationCenter.xClusterCollections.length(); i++) {
        // In case of TOP_N, filter every existant experiment for the top n most expressed genes
        switch (analysisConfigModel.usedFilterMode) {
            case AnalysisFilterMode::TOP_N:
                filteredXClusterCollections.append(Helper::findTopNMostExpressedGenes(this->informationCenter.xClusterCollections.at(i), analysisConfigModel.numberOfGenesToUse));
                break;

            case AnalysisFilterMode::MANUAL:
                filteredXClusterCollections.append(Helper::filterExpressedGenesAccordingToFilters(this->informationCenter.xClusterCollections.at(i), this->informationCenter.completeSetsOfGeneIDsPerDataset.at(i), analysisConfigModel));
                break;

            case AnalysisFilterMode::AUTOMATIC:
                break;
        }
    }

    // Correlate the datasets with the given cell type markers in separate threads
    this->correlateDatasets(filteredXClusterCollections, this->informationCenter.cellMarkersForTypes);
    qDebug() << "Finished correlating. Gathering information";

    // Gather and save the information from the correlation from the different threads
    this->saveInformationAfterCorrelatingFinished();
    qDebug() << "Saving correlation data successfull.";

    // Report that the last correlation thread has finished to the main window
    emit finishedCorrelating(this->informationCenter);

    // Before a new analysis can be made, the resulting data of the old analysis has to be deleted
    this->needsCleaning = true;
}

//void Coordinator::on_runAnalysis(QVector<QVector<FeatureCollection>> allClustersFromAllDatasetsWithGeneExpressions) {
//    qDebug() << "Coordinator: Received signal for run analyis.";

//    if (this->needsCleaning)
//        this->cleanData();

//    qDebug() << "Correlating datasets";

//    // Correlate the datasets with the given cell type markers in separate threads
//    this->correlateDatasets(allClustersFromAllDatasetsWithGeneExpressions, informationCenter.cellMarkersForTypes);
//    qDebug() << "Finished correlating. Gathering information";

//    // Gather and save the information from the correlation from the different threads
//    this->saveInformationAfterCorrelatingFinished();
//    qDebug() << "Saving correlation data successfull.";

//    // Report that the last correlation thread has finished to the main window
//    emit finishedCorrelating(this->informationCenter);

//    // Before a new analysis can be made, the resulting data of the old analysis has to be deleted
//    this->needsCleaning = true;
//}


/**
 * @brief Coordinator::on_geneExpressionDataRequested - The AnalysisTab has requested the gene expression data - Send it
 */
void Coordinator::on_geneExpressionDataRequested() {
    emit this->sendGeneExpressionData(this->informationCenter.xClusterCollections, this->informationCenter.completeSetsOfGeneIDsPerDataset);
}

// ###################################### INTERACTION WITH MAIN WINDOW ###########################################

/**
 * @brief Coordinator::on_projectFileUploaded
 * @param filePath
 */
void Coordinator::on_projectFileUploaded(const QStringList filePath) {
    qDebug() << "on_projectFileUploaded: received" << filePath;
}
