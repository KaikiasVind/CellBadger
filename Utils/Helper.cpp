#include "Helper.h"

#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QFile>


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


// ########################################### GUI ############################################

/**
 * @brief openFileDialog - Opens a open file dialog specific to files with given type
 * @param parent - Parent widget this file dialog should be placed on
 * @param validMimeTypeExtensions - StringList that contains the valid file types that the dialog shows
 * @param isAcceptsMultipleFiles - Should the user be able to select multiple files for upload
 * @return - List of Strings representing the file paths for the selected files
 */
QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles) {
    QFileDialog fileDialog(parent);
    fileDialog.setDirectory(QDir::home());
    fileDialog.setMimeTypeFilters(validMimeTypeExtensions);

    if (isAcceptsMultipleFiles)
        fileDialog.setFileMode(QFileDialog::ExistingFiles);
    else
        fileDialog.setFileMode(QFileDialog::ExistingFile);

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


// #################################### FILTERING #####################################


/**
 * @brief popTopNMostExpressedGenes - Finds the top n genes with the highest fold changes
 * @param experiment - List of FeatureCollections containing the to sort genes
 * @param numberOfGenesToPop - Number of genes the resulting FeatureCollections are going to have
 * @return - List of FeatureCollections with only the top n genes with the highest fold changes
 */
QVector<FeatureCollection> findTopNMostExpressedGenes(const QVector<FeatureCollection> experiment, int numberOfGenesToPop) {
    QVector<FeatureCollection> filteredExperiment;
    filteredExperiment.reserve(experiment.length());

    // Go through all collections of the experiment and p
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

}

