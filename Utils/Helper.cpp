#include "Helper.h"

#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QDebug>

#include "ExportDialog.h"


namespace Helper {

// ########################################### MISC ############################################

/**
 * @brief chopFileName - Chops the file name from the file path
 * @param filepath - Absolute file path
 * @param isKeepExtension - should the extension ".X" be kept on the file name
 * @return - Pure file name with or withour file extension
 */
QString chopFileName(QString filepath) {
    // Split by os specific directory separater.
    // The last entry is the file name with extension

    QString fileNameWithExtension;

    // Windows directory separators dont seem to work with QDir::separator
    if (filepath.contains("/"))
       fileNameWithExtension = filepath.split("/").last();
    else
        fileNameWithExtension = filepath.split(QDir::separator()).last();

    // Split extension and file name. The first one is the file name
    return fileNameWithExtension.split(".").first();
}


/**
 * @brief getFileNames - Chop the paths from the file paths and only return the file names
 * @param filePaths - List of paths to files
 * @return - List of names to files
 */
QStringList getFileNames(const QStringList filePaths) {
    QStringList fileNames;
    fileNames.reserve(filePaths.length());

    std::transform(filePaths.begin(), filePaths.end(), std::back_inserter(fileNames), chopFileName);

    return fileNames;
}


/**
 * @brief exitIfErrorOnOpen - Exit the program if the file cannot be opened without error
 * @param file - File to be opened
 * @param filePath - Path to the file that is to be opened
 */
void exitIfErrorOnOpen(QFile * file, const QString filePath) {
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "CSV READER:" << filePath << "-" << file->errorString();
        exit(1);
    }
}


// ########################################### GUI ############################################

/**
 * @brief openFileDialog - Opens a open file dialog specific to files with given type
 * @param parent - Parent widget this file dialog should be placed on
 * @param validMimeTypeExtensions - StringList that contains the valid file types that the dialog shows
 * @param isAcceptsMultipleFiles - Should the user be able to select multiple files for upload
 * @return - List of Strings representing the file paths for the selected files
 */
QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, QFileDialog::FileMode fileMode) {
    QFileDialog fileDialog(parent);
    fileDialog.setDirectory(QDir::home());
    fileDialog.setMimeTypeFilters(validMimeTypeExtensions);

    fileDialog.setFileMode(fileMode);

    QStringList fileNames;
    if (fileDialog.exec())
        fileNames = fileDialog.selectedFiles();

    return fileNames;
}


/**
 * @brief saveFileDialog - Opens a save file dialog specific to files with given type
 * @param parent - Parent widget this file dialog should be placed on
 * @param validMimeTypeExtensions - StringList that contains the valid file types that the dialog shows
 * @return - List of Strings representing the file paths for the selected files
 */
QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions) {
    return QFileDialog::getSaveFileName(parent, description, QDir::home().path(), validMimeTypeExtensions);
}


/**
 * @brief openExportWidgetWithPlot - Takes the given plot and opens it in an ExportDialog
 * @param chart - A QChartView * that is transfered onto an ExportDialog
 */
void openExportWidgetWithPlot(QtCharts::QChartView * chart) {
    ExportDialog * exportDialog = new ExportDialog();
    exportDialog->addPlot(chart);
    exportDialog->show();
}


// #################################### FILTERING #####################################


/**
 * @brief popTopNMostExpressedGenes - Finds the top n genes with the highest fold changes
 * @param experiment - List of FeatureCollections containing the to sort genes
 * @param numberOfGenesToPop - Number of genes the resulting FeatureCollections are going to have
 * @return - List of FeatureCollections with only the top n genes with the highest fold changes
 */
QVector<FeatureCollection> findTopNMostExpressedGenes(const QVector<FeatureCollection> experiment, const int numberOfGenesToPop) {
    QVector<FeatureCollection> filteredExperiment;
    filteredExperiment.reserve(experiment.length());

    // Go through all collections of the experiment and pop the top n genes
    for (FeatureCollection collection : experiment) {
        FeatureCollection filteredCollection(collection.ID);

        // Get all features from the current collection
        QVector<Feature> features = collection.getFeatures();

        // Make sure the number of genes to pop doesn't excell the number of genes in the collection
        if (numberOfGenesToPop < features.length()) {

            // Sort the genes according to their fold changes
            std::sort(features.begin(), features.end(),
                  [](Feature featureA, Feature featureB) { return featureA.foldChange > featureB.foldChange; });

            // then pop the first n genes and resize the list
            features.resize(numberOfGenesToPop);
            features.squeeze();
        }

        // and add the filtered features to the new collection
        for (int i = 0; i < features.length(); i++)
            filteredCollection.addFeature(features.at(i));

        // add the completed collection to the list and continue with the next collection
        filteredExperiment.append(filteredCollection);
    }

    return filteredExperiment;
}


/**
 * @brief filterExpressedGenesAccordingToFilters - Filter out the cluster's genes in the given experiment according to the cut-offs in the given AnalysisConfigModel
 * @param experiment - List of FeatureCollections containing the to sort genes
 * @param analysisConfigModel - Comined configuration and cut-offs to be used for the filtering process
 * @return - List of FeatureCollections with genes filtered out that do not apply to the cut-offs in the given AnalysisConfigModel
 */
QVector<FeatureCollection> filterExpressedGenesAccordingToFilters(const QVector<FeatureCollection> experiment, const QStringList completeGeneIDs, const AnalysisConfigModel analysisConfigModel) {
    QVector<FeatureCollection> filteredExperiment;
    filteredExperiment.reserve(experiment.length());

    // Create as many new collections as there were old ones
    for (FeatureCollection collection : experiment)
        filteredExperiment.append(FeatureCollection(collection.ID));

    for (QString featureID : completeGeneIDs) {
        QVector<Feature> featuresToBeAdded;

        int numberOfClustersWithGenesWithValidRPMValues = 0,
            numberOfClustersWithGenesWithValidRawCounts = 0,
            numberOfClustersWithGenesWithValidFoldChanges = 0;

        for (int i = 0; i < experiment.length(); i++) {
            Feature feature = experiment.at(i).getFeature(featureID);

            // Bools corresponding to whether the current feature matches given cut-offs
            bool featureMatchesRPMCutOff = false;
            bool featureMatchesRawCountCutOff = false;
            bool featureMatchesFoldChangeCutOff = false;

            // Check whether the current feature matches all cut-offs
            // Should the current feature not be expressed in the current cluster, every value is left as false
            if (feature.ID.compare("nAn") != 0) {
                featureMatchesRPMCutOff = true;
                featureMatchesRawCountCutOff = (feature.count >= analysisConfigModel.minRawCount) && (feature.count <= analysisConfigModel.maxRawCount);
                featureMatchesFoldChangeCutOff = (feature.foldChange >= analysisConfigModel.minFoldChange) && (feature.foldChange <= analysisConfigModel.maxFoldChange);
            }

            // Add the current feature to the cluster
            // Should the current fature not be expressed in the current cluster, the empty feature is added (which is later deleted)
            // This is done to ensure the correct order of the features to the correct clusters
            featuresToBeAdded.append(feature);

            // Note the matched cut-offs
            if (featureMatchesRPMCutOff)
                numberOfClustersWithGenesWithValidRPMValues++;

            if (featureMatchesRawCountCutOff)
                numberOfClustersWithGenesWithValidRawCounts++;

            if (featureMatchesFoldChangeCutOff)
                numberOfClustersWithGenesWithValidFoldChanges++;
        }

        // Check if the minimum number of clusters with valid feature cut-offs is met for the current gene
        bool isNumberOfValidRPMValuesClustersMet = true,
             isNumberOfValidRawCountClustersMet = false,
             isNumberOfValidFoldChangeClustersMet = false;

        // #################################################### RAW COUNTS ####################################################
        if (analysisConfigModel.includeRawCountInAtLeast) {
            isNumberOfValidRawCountClustersMet = numberOfClustersWithGenesWithValidRawCounts >= analysisConfigModel.rawCountInAtLeast;
        } else {
            isNumberOfValidRawCountClustersMet = numberOfClustersWithGenesWithValidRawCounts > 0;
        }

        // #################################################### FOLD CHANGES ##################################################
        if (analysisConfigModel.includeFoldChangeInAtLeast) {
            isNumberOfValidFoldChangeClustersMet = numberOfClustersWithGenesWithValidFoldChanges >= analysisConfigModel.foldChangeInAtLeast;
        } else {
            isNumberOfValidFoldChangeClustersMet = numberOfClustersWithGenesWithValidFoldChanges > 0;
        }

        if (isNumberOfValidRPMValuesClustersMet && isNumberOfValidRawCountClustersMet && isNumberOfValidFoldChangeClustersMet) {

            // If so, go through all to-be-added features
            for (int i = 0; i < featuresToBeAdded.length(); i++) {

                // If the feature is only a placeholder, continue with the next one
                bool isFeatureEmpty = featuresToBeAdded.at(i).ID.compare("nAn") == 0;
                if (isFeatureEmpty)
                    continue;

                // If it is a real feature add it to the correct cluster
                filteredExperiment[i].addFeature(featuresToBeAdded.at(i));
            }

        } else {
            // If the current "row" / feature does not have enough valid clusters, drop the entire "row" / feature
            continue;
        }
    }

    return filteredExperiment;
}

}

