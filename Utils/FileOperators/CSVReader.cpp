#include "CSVReader.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QStringList>
#include <QList>

#include "BioModels/FeatureCollection.h"
#include "BioModels/Celltype.h"

namespace CSVReader {

/**
 * @brief CSVReader::getClusterFeatureExpressions
 * @param csvFilePath
 * @param cutOff
 * @return
 */
QVector<FeatureCollection> getClusterFeatureExpressions(QString csvFilePath, double cutOff) {

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
    QVector<FeatureCollection> clustersWithExpressedFeatures;

    int numberOfColumns = splitLine.length();
    // The cellranger cluster feature expression file is segmented into 3 rows per cluster
    // with two lines in the very beginning for the feature ID and for the feature name -> Hence numberOfColumns - 2
    int numberOfClusters = (numberOfColumns - 2) / 3;

    // Add the culumn numbers for the cluster mean counts (always the first of the three columns per cluster)
    // and a new String list for each cluster. This list will later be filled with expressed features
    QVector<int> clusterColumnNumbers(numberOfClusters);
    for (int i = 0; i < numberOfClusters; i++) {
        clusterColumnNumbers[i] = (i * 3 + 2);
        QString clusterID = QString("Cluster").append(QString::number(i));
        FeatureCollection cluster(clusterID);
        clustersWithExpressedFeatures.append(cluster);
    }

    // Start parsing cluster file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split(',');

        // Check the expression for each feature in the clusters and add the feature in case its expressed
        for (int i = 0; i < numberOfClusters; i++) {
            double featureMeanCount = splitLine.at(clusterColumnNumbers[i]).toDouble();
            bool isFeatureExpressed = featureMeanCount > cutOff;

            // Get feature name and append to the correct cluster list
            if (isFeatureExpressed) {
                QString featureID = splitLine.at(1).toUpper();
                clustersWithExpressedFeatures[i].addFeature(featureID, featureMeanCount);
            }
        }
    }

    return clustersWithExpressedFeatures;
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

}
