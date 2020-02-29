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

/**
 * @brief getClusterFeatureExpressionFoldChanges
 * @param csvFilePath
 * @param cutOff
 * @return
 */
QVector<FeatureCollection> getClusterFeatureExpressionFoldChanges(QString csvFilePath, double cutOff) {

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
        clusterColumnNumbers[i] = (i * 3);
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

            // Read the log 2 fold change of the feature
            // This represents the expression rate of the feature relative to all other clusters
            double featureLog2FoldChange = splitLine.at(clusterColumnNumbers[i]).toDouble();

            // Calculate the original - non log fold change
            // A high value corresponds to a high expression rate in comparison to other clusters
            // Analog, a low value corresponds to a low expression rate in comparison to other clusters
            double featureFoldChange = Math::invertLog(2, featureLog2FoldChange);

            // The abs function is used here to check whether the fold change is high enough in any direction
            bool isFeatureFoldChangeSignificant = abs(featureFoldChange) > cutOff;

            // Get feature name and append to the correct cluster list
            if (isFeatureFoldChangeSignificant) {
                QString featureID = splitLine.at(1).toUpper();
                clustersWithSignificantFeatureFoldChanges[i].addFeature(featureID, featureFoldChange);
            }
        }
    }

    return clustersWithSignificantFeatureFoldChanges;
}


/**
 * @brief CSVReader::getCellTypeMarkers
 * @param csvFilePath
 * @return
 */
QVector<CellType> getCellTypesWithMarkers(QString csvFilePath) {
    // Open file
    QFile csvFile(csvFilePath);

    // In case of problems while reading the file
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << "CSV READER:" << csvFilePath << "-" << csvFile.errorString();
        exit(1);
    }

    // Skip title line
    QByteArray line = csvFile.readLine();
    QList<QByteArray> splitLine = line.split('\t');

    QVector<CellType> cellTypesWithMarkers;

    // Start parsing cell marker file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split('\t');

        QString cellTypeID = splitLine[5], //REMEMBER Is it possible to remove these hard coded column numbers?
                tissueTypeID = splitLine[1],
                cellMarkers = splitLine[7].toUpper();

        QStringList separateCellMarkers = cellMarkers.split(", ");

        CellType newCellType(cellTypeID, tissueTypeID, separateCellMarkers);
        cellTypesWithMarkers.append(newCellType);
    }

    return cellTypesWithMarkers;
}


/**
 * @brief DEPRECATED! - Sort marker-file from tissue / cell -> marker to marker -> tissue /cell
 * @param csvFilePath - Source path of the cell marker file
 * @return Hash of cell marker to list of every tissue / cell type.
 */
QHash <QString, QVector<QPair<QString, QString>>> sortCsvByMarker(QString csvFilePath) {

    // Open file
    QFile csvFile(csvFilePath);

    // Throw error in case opening the file fails
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << "CSV READER:" << csvFilePath << "-" << csvFile.errorString();
        exit(1);
    }

    // List of QPair<Marker, Tissues>
    QHash <QString, QVector<QPair<QString, QString>>> seenMarkers;

    // Skip title line
    QByteArray line = csvFile.readLine();
    QList<QByteArray> splitLine = line.split(',');

    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        QList<QByteArray> splitLine = line.split('\t');

        QString tissueType = splitLine.at(1),
                cellType = splitLine.at(5),
                cellMarker = splitLine.at(7).toUpper();

        // If the cell marker field contains multiple entries split them up
        QStringList singleCellMarkers = cellMarker.split(", ");

        // Go through complete list of cell markers -> May be just one
        for (QString cellMarker : singleCellMarkers) {
            // Get list tissue-celltype pairs of the marker that has been registered before
            QVector<QPair<QString, QString>> existingCellAndTissueTypes = seenMarkers.value(cellMarker);

            // If the marker hasn't been registered before
            if (!existingCellAndTissueTypes.isEmpty()) {
                // add the newly seen cell type for te cell marker to the already existing list
                existingCellAndTissueTypes.append(qMakePair(cellType, tissueType));

                // and add the updated list as a new cell-marker-cell-type-list-hash
                seenMarkers.insert(cellMarker, existingCellAndTissueTypes);
            } else {
                // Create new list with single cell type.
                QVector<QPair<QString, QString>> cellTypes;
                cellTypes.append(qMakePair(cellType, tissueType));
                seenMarkers.insert(cellMarker, cellTypes);
            }
        }
    }

    return seenMarkers;
}

/**
 * @brief getTissueGeneExpression
 * @param csvFilePath
 */
QVector<FeatureCollection> getTissuesWithGeneExpression(QString csvFilePath, double cutOff) {

    // Open file
    QFile csvFile(csvFilePath);

    // Throw error in case opening the file fails
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << "CSV READER:" << csvFilePath << "-" << csvFile.errorString();
        exit(1);
    }

    // tissueIDs is needed here to know the amount of features and
    // to add them later to the FeatureCollection
    QStringList tissueIDs;
    QVector<FeatureCollection> tissues;

    // Create a list to collect the ID of every gene that is expressed at least in one cluster.
    // This is neccessary to have a complete list of existing gene IDs for comparison between clusters
    // with differing expressed genes
    FeatureCollection completeGeneIDCollection("completeGeneIDCollection");
    QStringList completeGeneIDs;

    // The tissue names start at column 2
    int tissueIDsOffset = 2;

    // Get title line with tissue names
    QByteArray line = csvFile.readLine();
    QList<QByteArray> splitLine = line.split('\t');

    // and add them to the list
    for (int i = tissueIDsOffset; i < splitLine.length(); i++) {
        QString tissueID = splitLine[i];
        FeatureCollection tissue(tissueID);
        tissues.append(tissue);
    }

    int numberOfTissues = tissues.length();

    // Go through the rest of the file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split('\t');

        for (int i = tissueIDsOffset; i < numberOfTissues + tissueIDsOffset; i++) {
            QString featureID = splitLine[1].toUpper();
            double featureExpressionCount = splitLine[i].toDouble();
            bool isFeatureExpressed = featureExpressionCount > cutOff;

            // Add gene ID to the list of seen gene ids (see above for further information)
            if (featureExpressionCount > 1.0) {
                completeGeneIDCollection.addFeature(featureID, -1);
            }

            // Add expressed feature to tissue
            if (isFeatureExpressed) {
                tissues[i - tissueIDsOffset].addFeature(featureID, featureExpressionCount);
            }
        }
    }

    // Remove all duplicates that were created due to genes being expressed in multiple clusters
    completeGeneIDs.removeDuplicates();

    // Generate a feature collection out of the collected gene IDs for easy transition
    // REMEMBER: This is not clean
    for (QString geneID : completeGeneIDs) {
        completeGeneIDCollection.addFeature(geneID, -1);
    }

    return tissues;
}

///**
// * @brief getSensitivityAndSpecicifityForMarkers
// * @param csvFilePath
// * @return
// */
//QHash <QString, QVector<double>> getUIAndSensitivityAndSpecicifityForMarkers(QString csvFilePath) {

//    // Open fil
//    QFile csvFile(csvFilePath);

//    // Throw error in case opening the file fails
//    if (!csvFile.open(QIODevice::ReadOnly)) {
//        qDebug() << "CSV READER:" << csvFilePath << "-" << csvFile.errorString();
//        exit(1);
//    }

//    QHash <QString, QVector<double>> markersWithUIAndSensitivityAndSpecicifity;

//    // Skip the title line
//    QByteArray line = csvFile.readLine();
//    QList<QByteArray> splitLine;

//    // Go through the rest of the file
//    while (!csvFile.atEnd()) {
//        line = csvFile.readLine();
//        splitLine = line.split('\t');

//        // If marker is a mouse only marker, skip it
//        bool isMouseOnlyMarker = !splitLine[0].contains("Hs");
//        if (isMouseOnlyMarker)
//            continue;

//        // Otherwise extract data from line
//        QString markerID = splitLine[1];
//        double markerUbiquitousnessIndex = splitLine.at(2),
//               markerSensitivity = splitLine[3],
//               markerSpecicifity = splitLine[5];

//        // Bundle the three indizes together
//        QVector<double> markerUIAndSensitivityAndSpecicifity;
//        markerUIAndSensitivityAndSpecicifity.append(markerUbiquitousnessIndex);
//        markerUIAndSensitivityAndSpecicifity.append(markerSensitivity);
//        markerUIAndSensitivityAndSpecicifity.append(markerSpecicifity);

//        // And add them to the hash map that is going to be returned after parsing
//        markersWithUIAndSensitivityAndSpecicifity[markerID] = markerUIAndSensitivityAndSpecicifity;
//    }

//    return markersWithUIAndSensitivityAndSpecicifity;
//}

}
