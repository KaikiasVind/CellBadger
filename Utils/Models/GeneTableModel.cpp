#include "GeneTableModel.h"

#include <QDebug>
#include <cmath>

#include "BioModels/FeatureCollection.h"
#include "Utils/Helper.h"


/**
 * @brief GeneTableModel::GeneTableModel - A model that inherits QAbstractTableModel and is used to display gene expression in all clusters
 * @param geneExpressions - List of FeatureCollections (clusters) with expressed genes
 * @param completeGeneIDs - Complete list of all genes that have been seen in any above cluster
 * @param parent - Parent widget
 */
//GeneTableModel::GeneTableModel(const QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs, QObject * parent):
GeneTableModel::GeneTableModel(const QVector<std::tuple<QString, QVector<double>, double>> allGenesWithExpressionCountsInAllClusters,
                               const QStringList completeGeneIDs, const QStringList clusterNames, QObject * parent) :
    QAbstractTableModel(parent)
{
    this->allGenesWithExpressionCountsInAllClusters = allGenesWithExpressionCountsInAllClusters;
    this->completeGeneIDs = completeGeneIDs;
    this->numberOfClusters = clusterNames.length();
    this->clusterNames = clusterNames;
}


/**
 * @brief GeneTableModel::rowCount - Fetch the number of rows that the gene expression table shows,
 * @param parent - Parent widget (unused)
 * @return - Number of rows of the table (number of genes + title row)
 */
int GeneTableModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return this->completeGeneIDs.length();
}


/**
 * @brief GeneTableModel::columnCount - Fetch the number of columns the gene expressions shows.
 * @param parent - Parent widget (unused)
 * @return - Number of columns of the table (number of clusters * 2) + gene id column + mean count column
 */
int GeneTableModel::columnCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return (this->numberOfClusters * 2) + 2;
}


/**
 * @brief GeneTableModel::data - Influences how the data is presented in the table.
 * @param index - Index of the current cell.
 * @param role - Information type that is requested currently, e.g. content or alignment of the cell
 * @return - QVariant with the correct way the data should be presented.
 */
QVariant GeneTableModel::data(const QModelIndex & index, int role) const {

    // Block invalid indices
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= this->rowCount() || index.row() < 0) {
        return QVariant();
    }

    // Fetch the data from the underlying data models and report it to the table
    if (role == Qt::DisplayRole) {
        if (this->columnCount() < this->clusterNames.length())
            qDebug() << "column couhnt < clusternames.length()";
        const std::tuple<QString, QVector<double>, double> & geneWithExpressions = this->allGenesWithExpressionCountsInAllClusters.at(index.row());

        // Calculate the correct index to retrieve the correct cluster name
        int correctClusterIndex = 0;
        if (0 < index.column() < (this->clusterNames.length() * 2) + 1)
            correctClusterIndex  = ceil((double) index.column() / 2) - 1;

//        Feature currentGene = this->clustersWithGeneExpressions.at(correctIndex).getFeature(currentGeneID);
//        qDebug() << "correct index:" << correctIndex;
//        qDebug() << "column:" << index.column();
//        qDebug() << "ceil:" << correctClusterIndex;

        QString currentGeneID = this->completeGeneIDs.at(index.row());

//        qDebug() << "Col:" << index.column();
//        qDebug() << "Correct cluster index:" << correctClusterIndex;

        if (index.column() == 0) {
//            qDebug() << "gene";
            return this->completeGeneIDs.at(index.row());
        } else if (index.column() == (this->numberOfClusters * 2) + 1) {
//            qDebug() << "mean";

            // Calculate the mean
            double meanRawCount = 0;
            for (FeatureCollection cluster : this->clustersWithGeneExpressions) {
                double rawCountForCurrentGene = cluster.getFeature(currentGeneID).count;
                if (rawCountForCurrentGene != -1)
                    meanRawCount += rawCountForCurrentGene;
            }

            return meanRawCount / this->numberOfClusters;
        } else {
//            qDebug() << "raw count / fold change.";

            // Search for the current gene with its ID
            Feature currentGene = this->clustersWithGeneExpressions.at(correctClusterIndex).getFeature(currentGeneID);

            bool isCurrentGeneNotFoundInCluster = false;

            // If the gene has not been found return 0 for the count and fold change
            if (currentGene.ID.compare("nAn") == 0)
                isCurrentGeneNotFoundInCluster = true;

            // Otherwise return the corresponding count or fold change value
            if (index.column() % 2 == 1) {
                if (isCurrentGeneNotFoundInCluster)
                    return 0;
                else
                    return currentGene.count;
            } else {
                if (isCurrentGeneNotFoundInCluster)
                    return 1;
                else
                    return currentGene.foldChange;
            }
        }

    // Decide which cell should be aligned in which way
    } else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0) // || index.column() == this->columnCount() - 1)
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        else
            return Qt::AlignCenter;
    }

    return QVariant();
}


/**
 * @brief GeneTableModel::headerData - Set the header labels
 * @param section - Current vertical header cell index
 * @param orientation - Orientation
 * @param role - Role
 * @return - The label that the current vertical header cell should show
 */
QVariant GeneTableModel::headerData(int section, Qt::Orientation orientation, int role) const {

    // Only care about display options here
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Gene");
        } else if (section == (this->numberOfClusters * 2) + 1) {
            return tr("mean");
        } else if ((section % 2) == 1) {
            QString currentClusterName = this->clusterNames.at(ceil((double) section / 2) - 1);
            return tr(qPrintable(currentClusterName + " raw count"));
        } else {
            QString currentClusterName = this->clusterNames.at(ceil((double) section / 2) - 1);
            return tr(qPrintable(currentClusterName + " fold change"));
        }
    }

    return section + 1;
}

