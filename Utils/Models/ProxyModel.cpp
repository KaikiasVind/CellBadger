#include "ProxyModel.h"

#include <QSortFilterProxyModel>
#include <QDebug>


/**
 * @brief ProxyModel::ProxyModel
 * @param parent
 */
ProxyModel::ProxyModel(QObject * parent) : QSortFilterProxyModel(parent),
    minRawCount (0.0), maxRawCount (0.0), minFoldChange(0.0), maxFoldChange(0.0), rawCountinAtLeast(0), includeRawCountInAtLeast(false),
    foldChangeInAtLeast(0), includeFoldChangeInAtLeast(false), searchedGeneIDs(QStringList()) {}


/**
 * @brief ProxyModel::ProxyModel
 * @param rowCount
 * @param colCount
 * @param parent
 */
ProxyModel::ProxyModel(int rowCount, int colCount, QObject * parent) : QSortFilterProxyModel(parent),
    rowCount (rowCount), columnCount (colCount), minRawCount (0.0), maxRawCount (0.0), minFoldChange(0.0), maxFoldChange(0.0),
  rawCountinAtLeast(0), includeRawCountInAtLeast(false),foldChangeInAtLeast(0), includeFoldChangeInAtLeast(false),
  searchedGeneIDs(QStringList()) {}


/**
 * @brief ProxyModel::filterAcceptsRow - Evaluates whether the selected row meets the given requirements
 * @param source_row - The selected row
 * @param source_parent - The parent widget
 * @return - Returns whether the selected row meets the given requirements
 */
bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {

    QVector<QModelIndex> clusterIndices;
    clusterIndices.reserve(this->columnCount);
//    qDebug() << "min:" << this->minRawCount;
//    qDebug() << "max:" << this->maxRawCount;

    for (int i = 0; i < columnCount; i++) {
        clusterIndices.append(this->sourceModel()->index(source_row, i, source_parent));
    }

//    int numberOfClustersWithValidFoldChange = 0;
    int numberOfClustersWithValidRawCount = 0;
    for (int i = 0; i < clusterIndices.length(); i++) {

        // ################################ GENE-IDS ################################
        // If there are gene IDs the table should be filtered by check the selected rows for those gene IDs
        if (!this->searchedGeneIDs.isEmpty()) {

            QModelIndex currentRowGeneIDModelIndex = this->sourceModel()->index(source_row, 0);
            QString currentRowGeneID = this->sourceModel()->data(currentRowGeneIDModelIndex).toString().toLower();

            // Go through the list of given gene IDs and search for the current row's gene ID
            // Accept the row if one has been found
            QStringList unseenGeneIDs = this->searchedGeneIDs;
            bool isIDHasBeenSeen = false;
            for (int i = 0; i < unseenGeneIDs.length(); i++) {
                if (currentRowGeneID.contains(this->searchedGeneIDs.at(i))) {
                    isIDHasBeenSeen = true;
                }
            }
            if (!isIDHasBeenSeen)
                return false;
        }

        // ################################ CUT-OFFS ################################
        double currentCellValue = this->sourceModel()->data(clusterIndices.at(i)).toDouble();

        bool isMinRawCountCutOffMet = (currentCellValue == this->minRawCount || currentCellValue > this->minRawCount);
        bool isMaxRawCountCutOffMet = (currentCellValue == this->maxRawCount || currentCellValue < this->maxRawCount);

        if (isMinRawCountCutOffMet && isMaxRawCountCutOffMet)
            numberOfClustersWithValidRawCount += 1;
    }

    // ################################ IN-AT-LEAST-OFFS #############################
    if (this->includeRawCountInAtLeast) {
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
    qDebug() << "set max:" << maxRawCount;
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


/**
 * @brief ProxyModel::setSearchedGeneIDs - Sets the list of gene IDs the table is filtered by
 * @param searchedGeneIDs - List of gene IDs for filtering
 */
void ProxyModel::setSearchedGeneIDs(QStringList searchedGeneIDs) {
    if (this->searchedGeneIDs != searchedGeneIDs)
        this->searchedGeneIDs = searchedGeneIDs;
    invalidateFilter();
}
