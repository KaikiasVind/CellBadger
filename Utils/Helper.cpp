#include "Helper.h"

#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QDebug>

#include "Utils/Math.h"

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

// ########################################### MISC ############################################

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
QString openSaveFileDialog(QWidget * parent, QString validMimeTypeExtensions) {
    return QFileDialog::getSaveFileName(parent, "Save plot as png file", QDir::home().path(), validMimeTypeExtensions);
}

// ########################################### GUI ############################################


// #################################### BIOMODELS ####################################

/**
 * @brief getFeatureCollectionsAsGenes - Get the list of feature collections as list of genes with expression counts in all feature collections
 * @param featureCollections - List of feature collections from a file e.g.
 * @param completeGeneIDs - List of all genes that are of interest
 * @return - List of all given genes with corresponding expression counts in all given clusters
 */
QVector<std::tuple<QString, QVector<double>, double>> getFeatureCollectionsAsGenes(const QVector<FeatureCollection> featureCollections, const QStringList completeGeneIDs) {

    // Create a list that will hold all genes with all corresponding expression counts in all clusters
    QVector<std::tuple<QString, QVector<double>, double>> genesWithExpressionCountsInAllFeatureCollections;

    // Go through every gene and search for the gene's expression count in all feature collections
    for (QString geneID : completeGeneIDs) {

        // Create a new pair that will hold all expression counts of the gene in all feature collections
        std::tuple<QString, QVector<double>, double> geneWithExpressionCountsInAllFeatureCollections{geneID, {}, 0};

        // Go through all feature collections and append the current gene's expression count if found
        for (FeatureCollection featureCollection : featureCollections) {

            bool isGeneFound = false;

            // Search through all features in the current collection and append the expression count when found
            for (Feature feature : featureCollection.getFeatures()) {
                if (feature.ID.compare(geneID) == 0) {
                    std::get<1>(geneWithExpressionCountsInAllFeatureCollections).append(feature.count);
                    isGeneFound = true;
                    break;
                }
            }

            // If the gene has not been found, add zero as expression count for the feature collection
            if (!isGeneFound) {
                std::get<1>(geneWithExpressionCountsInAllFeatureCollections).append(0.0);
            }

            std::get<2>(geneWithExpressionCountsInAllFeatureCollections) = Math::mean(std::get<1>(geneWithExpressionCountsInAllFeatureCollections));
        }

        genesWithExpressionCountsInAllFeatureCollections.append(geneWithExpressionCountsInAllFeatureCollections);
    }

    return genesWithExpressionCountsInAllFeatureCollections;
}

// #################################### BIOMODELS ####################################
};
