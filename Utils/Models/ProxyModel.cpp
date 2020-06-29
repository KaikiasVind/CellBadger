#include "ProxyModel.h"

#include <QSortFilterProxyModel>

ProxyModel::ProxyModel(QObject * parent) : QSortFilterProxyModel(parent),
    minRawCount (0.0), maxRawCount (0.0), minFoldChange(0.0), maxFoldChange(0.0) {}


ProxyModel::ProxyModel(int rowCount, int colCount, QObject * parent) : QSortFilterProxyModel(parent),
    rowCount (rowCount), columnCount (colCount), minRawCount (0.0), maxRawCount (0.0), minFoldChange(0.0), maxFoldChange(0.0) {}


/**
 * @brief ProxyModel::filterAcceptsRow - Evaluates whether the selected row meets the given requirements
 * @param source_row - The selected row
 * @param source_parent - The parent widget
 * @return - Returns whether the selected row meets the given requirements
 */
bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {

    QVector<QModelIndex> clusterIndices;
    clusterIndices.reserve(this->columnCount);

    for (int i = 0; i < columnCount; i++) {
        clusterIndices.append(sourceModel()->index(source_row, i, source_parent));
    }

    int numberOfClustersWithValidRawCount = 0;
//    int numberOfClustersWithValidFoldChange = 0;
    for (int i = 0; i < clusterIndices.length(); i++) {

        if (sourceModel()->data(clusterIndices.at(i)).toDouble() > this->minRawCount &&
                sourceModel()->data(clusterIndices.at(i)).toDouble() < this->maxRawCount) {
            numberOfClustersWithValidRawCount += 1;
        }
    }

    if (!this->includeRawCountInAtLeast) {
        // If enough cluster raw counts have met the required cut-off accept the row
        if (numberOfClustersWithValidRawCount >= this->rawCountinAtLeast)
            return true;
    } else {
        if (numberOfClustersWithValidRawCount > 0)
            return true;
    }

    return false;
}


/**
 * @brief ProxyModel::headerData - Keeps the numbers of the vertical header ascending despite the order of the rows
 * @param section - Section
 * @param orientation - Orientation
 * @param role - Role
 * @return - Not - sorted vertical headers
 */
QVariant ProxyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return sourceModel()->headerData(section, orientation, role);
}


/**
 * @brief ProxyModel::setMinRawCount - Sets the min accepted raw count
 * @param minRawCount - Raw Count
 */
void ProxyModel::setMinRawCount(double minRawCount) {
    if (this->minRawCount != minRawCount)
        this->minRawCount = minRawCount;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setMaxRawCount - Set the max accepted raw count
 * @param maxRawCoun - Raw Countt
 */
void ProxyModel::setMaxRawCount(double maxRawCount) {
    if (this->maxRawCount != maxRawCount)
        this->maxRawCount = maxRawCount;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setMinFoldChange - Set the min accepted fold change
 * @param minFoldChange - Fold Change
 */
void ProxyModel::setMinFoldChange(double minFoldChange) {
    if (this->minFoldChange != minFoldChange)
        this->minFoldChange = minFoldChange;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setMaxFoldChange - Set the max accepted fold change
 * @param maxFoldChange - Fold Change
 */
void ProxyModel::setMaxFoldChange(double maxFoldChange) {
    if (this->maxFoldChange != maxFoldChange)
        this->maxFoldChange = maxFoldChange;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setRawCountInAtLeast - Sets the minimum number of cluster that should met the raw-count cut-offs
 * @param amount - Minimum number of clusters
 */
void ProxyModel::setRawCountInAtLeast(int amount) {
    if (this->rawCountinAtLeast != amount)
        this->rawCountinAtLeast = amount;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setIncludeRawCountInAtLeast - Sets whether the minimum amount of cluster sould be considered or ignorded (In case the gui element is deactivated)
 * @param state - Whether or not the number should be considered or ignored
 */
void ProxyModel::setIncludeRawCountInAtLeast(bool state) {
    if (this->includeRawCountInAtLeast != state)
        this->includeRawCountInAtLeast = state;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setFoldChangeInAtLeast - Sets the minimum number of cluster that should met the fold-change cut-offs
 * @param amount - Minimum number of clusters
 */
void ProxyModel::setFoldChangeInAtLeast(int amount) {
    if (this->foldChangeInAtLeast != amount)
        this->foldChangeInAtLeast = amount;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setIncludeFoldChangeInAtLeast - Sets whether the minimum amount of cluster sould be considered or ignorded (In case the gui element is deactivated)
 * @param state - Whether or not the number should be considered or ignored
 */
void ProxyModel::setIncludeFoldChangeInAtLeast(bool state) {
    if (this->includeFoldChangeInAtLeast != state)
        this->includeFoldChangeInAtLeast = state;
    invalidateFilter();
}
