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


/**
 * @brief getCorrectClusterIndex
 * @param column
 * @return
 */
int getCorrectClusterIndex(int column) {
    return ceil((double) column / 2) - 1;
}


/**
 * @brief calculateManRawCountForGene - Calculate the mean of the expression counts of the given gene in all clusters
 * @param geneID - Name of the given gene
 * @param clusters - All clusters maybe expressing the gene
 * @return - Mean of all found expression counts of the given gene in all given clusters
 */
double calculateMeanRawCountForGene(QString geneID, QVector<FeatureCollection> clusters) {
    double meanRawCount = 0;
    for (FeatureCollection cluster : clusters) {
        double rawCountForCurrentGene = cluster.getFeature(geneID).count;

        if (rawCountForCurrentGene != -1)
            meanRawCount += rawCountForCurrentGene;
    }

    return meanRawCount / clusters.length();
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
QString openSaveFileDialog(QWidget * parent, QString validMimeTypeExtensions) {
    return QFileDialog::getSaveFileName(parent, "Save plot as png file", QDir::home().path(), validMimeTypeExtensions);
}

}
