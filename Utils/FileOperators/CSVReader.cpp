#include "CSVReader.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QStringList>
#include <QList>
#include <math.h>

#include "BioModels/FeatureCollection.h"
#include "BioModels/Celltype.h"
#include "Utils/Math.h"

namespace CSVReader {


QVector<FeatureCollection> readCellTypesFromPanglaoDBFile(QString csvFilePath) {

    // Open file
    QFile csvFile(csvFilePath);

    // Throw error in case opening the file fails
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << "CSV READER:" << csvFilePath << "-" << csvFile.errorString();
        exit(1);
    }

    // Skip title line
    char columnDelimiter('\t');
    QByteArray line = csvFile.readLine();
    QList<QByteArray> splitLine = line.split(columnDelimiter);

    // Create list that will contain all cell types with all associated markers present in the given file
    QVector<FeatureCollection> cellTypes;

    // Start parsing cluster file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split(columnDelimiter);

        // ############################# FILTERING OUT INVALID VALUES #############################
        // Check for gene species
        QString species = splitLine[0];

        bool isMouseOnlyGene = !species.contains("Hs");

        // If the gene is annotated as only expressed in mice, skip it
        if (isMouseOnlyGene)
            continue;

        // Otherwise, extract other data from line
        QString geneEnsemblID = splitLine[0],
                geneID = splitLine[1],
                cellTypeID = splitLine[2],
                tissueType = splitLine[9];

        QString ubiquitousnessIndex = splitLine[4],
               geneSensitivity = splitLine[10],
               geneSpecifity = splitLine[12];

        bool isValuesInvalid = geneSensitivity == "NA" || geneSpecifity == "NA";

        // If at least one of the values is marked as not existing in the file, drop it
        if (isValuesInvalid)
            continue;

        // Otherwise parse the values
        double geneSensitivityValue = geneSensitivity.toDouble(),
               geneSpecifityValue = geneSpecifity.toDouble();

        bool isAtLeastOneValueZero = geneSensitivityValue == 0 || geneSpecifityValue == 0;

        // Zeros stand for nAn as well, so drop them
        if (isAtLeastOneValueZero)
            continue;

        // ############################# FILTERING OUT INVALID VALUES #############################

        // Check if a new cell type is met while file parsing -> 0 means equal strings
        bool isNewCellType = cellTypes.isEmpty() || cellTypes.last().ID.compare(cellTypeID) != 0;

        // If a new cell type is met, append it to the list of cell types
        if (isNewCellType) {
            FeatureCollection cellType(cellTypeID);
            cellTypes.append(cellType);
        }

        //REMEMBER: Do I need the UbIndex?
        cellTypes.last().addFeature(geneID, geneEnsemblID, geneSensitivityValue, geneSpecifityValue);
    }

    return cellTypes;
}


/**
 * @brief getClusterFeatureExpressionFoldChanges
 * @param csvFilePath
 * @param cutOff
 * @return
 */
QVector<FeatureCollection> read10xGenomicsClustersFromFile(QString csvFilePath, double meanCountCutOff, double foldChangeCutOff) {
    // Open file
    QFile csvFile(csvFilePath);

    // Throw error in case opening the file fails
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << "CSV READER:" << csvFilePath << "-" << csvFile.errorString();
        exit(1);
    }

    // Skip title line
    QByteArray line = csvFile.readLine();
    QList<QByteArray> splitLine = line.split(',');

    // Each cluster contains its expressed features
    QVector<FeatureCollection> clustersWithSignificantFeatureFoldChanges;

    int numberOfColumns = splitLine.length();

    // The cellranger cluster feature expression file is segmented into 3 rows per cluster
    // with two lines in the very beginning for the feature ID and for the feature name -> Hence numberOfColumns - 2
    int numberOfClusters = (numberOfColumns - 2) / 3;

    // Add the culumn numbers for the cluster fold changes (always the second of the three columns per cluster)
    // and a new String list for each cluster. This list will later be filled with expressed features
    QVector<int> clusterColumnNumbers(numberOfClusters);
    for (int i = 0; i < numberOfClusters; i++) {
        clusterColumnNumbers[i] = (2 + i * 3);
        QString clusterID = QString("Cluster").append(QString::number(i));
        FeatureCollection cluster(clusterID);
        clustersWithSignificantFeatureFoldChanges.append(cluster);
    }

    // Start parsing cluster file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split(',');

        // Check the fold change for each feature in each cluster and add the feature in case its high enough
        for (int i = 0; i < numberOfClusters; i++) {

            // Read the mean count of the feature
            //REMEMBER: Is this the expression or the UMI mean count?!
            double featureMeanCount = splitLine.at(clusterColumnNumbers[i]).toDouble();

            // Read the log 2 fold change of the feature (+1 because the mean count is always one column after the log2 fold change)
            // This represents the expression rate of the feature relative to all other clusters
            double featureLog2FoldChange = splitLine.at(clusterColumnNumbers[i] + 1).toDouble();

            // Calculate the original - non log fold change
            // A high value corresponds to a high expression rate in comparison to other clusters
            // Analog, a low value corresponds to a low expression rate in comparison to other clusters
            double featureFoldChange = Math::invertLog(2, featureLog2FoldChange);

            // The abs function is used here to check whether the fold change is high enough in any directions
            // REMEMBER: I removed it due to it leading to wrong results -> If the feature is strongly underrepresented, it is exactly NOT relevant for direct comparison with highly represented features
            bool isFeatureFoldChangeSignificant = featureFoldChange > foldChangeCutOff;
            bool isFeatureMeanCountSignificant = featureMeanCount > meanCountCutOff;

            // Get feature name and append to the correct cluster list
            if (isFeatureMeanCountSignificant && isFeatureFoldChangeSignificant) {
                QString featureEnsemblID = splitLine.at(0).toUpper();
                QString featureID = splitLine.at(1).toUpper();
                clustersWithSignificantFeatureFoldChanges[i].addFeature(featureID, featureEnsemblID, featureMeanCount, featureLog2FoldChange, featureFoldChange);
            }
        }
    }

    return clustersWithSignificantFeatureFoldChanges;
}


// DEPRECATED
///**
// * @brief CSVReader::getCellTypeMarkers
// * @param csvFilePath
// * @return
// */
//QVector<CellType> getCellTypesWithMarkers(QString csvFilePath, double meanCountCutOff, double foldChangeCutOff) {

//    // Open file
//    QFile csvFile(csvFilePath);

//    // In case of problems while reading the file
//    if (!csvFile.open(QIODevice::ReadOnly)) {
//        qDebug() << "CSV READER:" << csvFilePath << "-" << csvFile.errorString();
//        exit(1);
//    }

//    // Skip title line
//    QByteArray line = csvFile.readLine();
//    QList<QByteArray> splitLine = line.split('\t');

//    QVector<CellType> cellTypesWithMarkers;

//    // Start parsing cell marker file
//    while (!csvFile.atEnd()) {
//        line = csvFile.readLine();
//        splitLine = line.split('\t');

//        QString cellTypeID = splitLine[5], //REMEMBER Is it possible to remove these hard coded column numbers?
//                tissueTypeID = splitLine[1],
//                cellMarkers = splitLine[7].toUpper();

//        QStringList separateCellMarkers = cellMarkers.split(", ");

//        CellType newCellType(cellTypeID, tissueTypeID, separateCellMarkers);
//        cellTypesWithMarkers.append(newCellType);
//    }

//    return cellTypesWithMarkers;
//}

}
