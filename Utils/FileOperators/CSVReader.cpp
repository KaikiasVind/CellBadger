#include "CSVReader.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QStringList>
#include <QList>

CSVReader::CSVReader(){}


/**
 * @brief CSVReader::getClusterFeatureExpressions
 * @param csvFilePath
 * @param cutOff
 * @return
 */
QVector<QStringList> CSVReader::getClusterFeatureExpressions(QString csvFilePath, double cutOff) {

    // Open file
    QFile csvFile(csvFilePath);

    // Throw error in case opening the file fails
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << csvFile.errorString();
        exit(1);
    }

    // Skip title line
    QByteArray line = csvFile.readLine();
    QList<QByteArray> splitLine = line.split(',');

    // Each QStringList represents one cluster.
    // The QStringList is filled with the features that are expressed by the cluster
    QVector<QStringList> expressedFeaturesForClusters;

    int numberOfColumns = splitLine.length();
    // The cellranger cluster feature expression file is segmented into 3 rows per cluster
    // with two lines in the very beginning for the feature ID and for the feature name -> Hence numberOfColumns - 2
    int numberOfClusters = (numberOfColumns - 2) / 3;

    // Add the culumn numbers for the cluster mean counts (always the first of the three columns per cluster)
    // and a new String list for each cluster. This list will later be filled with expressed features
    QVector<int> clusterColumnNumbers(numberOfClusters);
    for (int i = 0; i < numberOfClusters; i++) {
        clusterColumnNumbers[i] = (i * 3 + 2);
        QStringList clusterFeatures;
        expressedFeaturesForClusters.append(clusterFeatures);
    }

    // Start parsing cluster file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split(',');

        // Check the expression for each feature in the clusters and add the feature in case its expressed
        for (int i = 0; i < numberOfClusters; i++) {
            double featureMeanCount = splitLine.at(clusterColumnNumbers[i]).toDouble();
            // A cutoff of 0 or > 0 is chosen here
            bool isFeatureExpressed = featureMeanCount > cutOff;

            // Get feature name and append to the correct cluster list
            if (isFeatureExpressed) {
                QString feature = splitLine.at(1);
                expressedFeaturesForClusters[i].append(feature);
            }
        }
    }

    return expressedFeaturesForClusters;
}


/**
 * @brief CSVReader::getCellTypeMarkers
 * @param csvFilePath
 * @return
 */
QVector<QPair<QPair<QString, QString>, QStringList>> CSVReader::getCellTypeMarkers(QString csvFilePath) {
    // Open file
    QFile csvFile(csvFilePath);

    // In case of problems while reading the file
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << csvFile.errorString();
        exit(1);
    }

    // Skip title line
    QByteArray line = csvFile.readLine();
    QList<QByteArray> splitLine = line.split('\t');

    QVector<QPair<QPair<QString, QString>, QStringList>> cellTypeMarkers;

    // Start parsing cell marker file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split('\t');

        QString cellType = splitLine[5], //REMEMBER Is it possible to remove these hard coded column numbers?
                tissueType = splitLine[1],
                cellMarkers = splitLine[7].toUpper();

        QStringList separateCellMarkers = cellMarkers.split(", ");

        cellTypeMarkers.append(qMakePair(qMakePair(cellType, tissueType), separateCellMarkers));
    }

    return cellTypeMarkers;
}


/**
 * @brief Sort marker-file from tissue / cell -> marker to marker -> tissue /cell
 * @param csvFilePath - Source path of the cell marker file
 * @return Hash of cell marker to list of every tissue / cell type.
 */
QHash <QString, QVector<QPair<QString, QString>>> CSVReader::sortCsvByMarker(QString csvFilePath) {

    // Open file
    QFile csvFile(csvFilePath);

    // Throw error in case opening the file fails
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << csvFile.errorString();
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
