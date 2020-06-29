#include "GeneTableModel.h"

#include <QDebug>

#include "BioModels/FeatureCollection.h"
#include "Utils/Helper.h"


/**
 * @brief GeneTableModel::GeneTableModel - A model that inherits QAbstractTableModel and is used to display gene expression in all clusters
 * @param geneExpressions - List of FeatureCollections (clusters) with expressed genes
 * @param completeGeneIDs - Complete list of all genes that have been seen in any above cluster
 * @param parent - Parent widget
 */
GeneTableModel::GeneTableModel(const QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs, QObject * parent):
    QAbstractTableModel(parent)
{
    this->completeGeneIDs = completeGeneIDs;
    this->numberOfClusters = geneExpressions.length();

    // Gather all cluster names
    for (int i = 0; i < geneExpressions.length(); i++)
        this->clusterNames.append(geneExpressions.at(i).ID);

    // Gather gene raw counts from all clusters
    this->allGenesWithExpressionCountsInAllClusters = Helper::getFeatureCollectionsAsGenes(geneExpressions, completeGeneIDs);
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
 * @return - Number of columns of the table (number of clusters + gene id column + mean count column)
 */
int GeneTableModel::columnCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return this->numberOfClusters + 2;
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
        const std::tuple<QString, QVector<double>, double> & geneWithExpressions = this->allGenesWithExpressionCountsInAllClusters.at(index.row());

        if (index.column() == 0) {
            return std::get<0>(geneWithExpressions);
        } else if (index.column() < this->numberOfClusters + 1) {
            return std::get<1>(geneWithExpressions).at(index.column() - 1);
        } else {
            return std::get<2>(geneWithExpressions);
        }

    // Decide which cell should be aligned in which way
    } else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0 || index.column() == this->columnCount() - 1)
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
        } else if (section == this->numberOfClusters + 1) {
            return tr("mean");
        } else {
            return tr(qPrintable(this->clusterNames.at(section - 1)));
        }
    }

    return section + 1;
}

