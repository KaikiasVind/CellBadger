#include "GeneTableModel.h"

#include <QDebug>
#include <cmath>

#include "BioModels/FeatureCollection.h"
#include "Utils/Math.h"


/**
 * @brief GeneTableModel::GeneTableModel - A model that inherits QAbstractTableModel and is used to display gene expression in all clusters
 * @param geneExpressions - List of FeatureCollections (clusters) with expressed genes
 * @param completeGeneIDs - Complete list of all genes that have been seen in any above cluster
 * @param parent - Parent widget
 */
GeneTableModel::GeneTableModel(const QVector<FeatureCollection> geneExpressions, const QStringList completeGeneIDs, const QStringList clusterNames, QObject * parent)
    : QAbstractTableModel(parent), clustersWithGeneExpressions(geneExpressions), completeGeneIDs(completeGeneIDs), clusterNames(clusterNames), numberOfClusters(clusterNames.length())
{}


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
        if (this->columnCount() < this->clusterNames.length())
            qDebug() << "column count < clusternames.length()";

        QString currentGeneID = this->completeGeneIDs.at(index.row());

        if (index.column() == 0) {
            return currentGeneID;

        } else if (index.column() == this->numberOfClusters + 1) {
            QVector<double> rowValues;
            rowValues.reserve(this->columnCount() - 2);

            // Gather all values in the current row
            for (int i = 1; i < this->columnCount() - 2; i++) {
                QModelIndex cellIndex = this->index(index.row(), i);
                double cellData = this->data(cellIndex).toDouble();
                rowValues.append(cellData);
            }

            // And calculate the mean for the current row
            return Math::mean(rowValues);

        } else {
            // Search for the current gene with its ID
            Feature currentGene = this->clustersWithGeneExpressions.at(index.column() - 1).getFeature(currentGeneID);

            bool isCurrentGeneNotFoundInCluster = false;

            // If the gene has not been found return 0 for the count and fold change
            if (currentGene.ID.compare("nAn") == 0)
                isCurrentGeneNotFoundInCluster = true;

            switch (this->currentlyShownDataType) {
                case Helper::ShownData::RPM:
                    return 1;
                break;

                case Helper::ShownData::RAW_COUNTS:
                    if (isCurrentGeneNotFoundInCluster)
                        return 0;
                    else
                        return currentGene.count;
                break;

                case Helper::ShownData::FOLD_CHANGES:
                    if (isCurrentGeneNotFoundInCluster)
                        return 1;
                    else
                        return currentGene.foldChange;
                break;
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
        } else if (section == (this->numberOfClusters + 1)) {
            return tr("mean");
        } else {
            QString currentClusterName = this->clusterNames.at(section - 1);
            return tr(qPrintable(currentClusterName));
        }
    }

    return section + 1;
}


/**
 * @brief GeneTableModel::getClusterNames
 * @return
 */
QStringList GeneTableModel::getClusterNames() const {
    return clusterNames;
}


/**
 * @brief GeneTableModel::setCurrentlyShownDataType - Set the data type that should be shown in the table view
 * @param dataTypeToShow - Enum representing the new data type
 */
void GeneTableModel::setCurrentlyShownDataType(const Helper::ShownData dataTypeToShow) {
    if (this->currentlyShownDataType == dataTypeToShow)
        return;
    this->currentlyShownDataType = dataTypeToShow;
}
