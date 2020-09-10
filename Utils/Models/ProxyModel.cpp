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
ProxyModel::ProxyModel(int rowCount, int colCount, double maxRawCount, double maxFoldChange, QObject * parent) : QSortFilterProxyModel(parent),
    rowCount (rowCount), columnCount (colCount), minRawCount (0.0), maxRawCount (maxRawCount), minFoldChange(0.0), maxFoldChange(maxFoldChange),
  rawCountinAtLeast(0), includeRawCountInAtLeast(false),foldChangeInAtLeast(0), includeFoldChangeInAtLeast(false),
  searchedGeneIDs(QStringList()) {};


/**
 * @brief ProxyModel::filterAcceptsRow - Evaluates whether the selected row meets the given requirements
 * @param source_row - The selected row
 * @param source_parent - The parent widget
 * @return - Returns whether the selected row meets the given requirements
 */
bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {

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
    QVector<QModelIndex> cellIndices;
    cellIndices.reserve(this->columnCount);

    for (int i = 0; i < columnCount; i++) {
        cellIndices.append(this->sourceModel()->index(source_row, i, source_parent));
    }

    int numberOfClustersWithValidTypeCount = 0;
    bool includeTypeCountInAtLeast;
    int typeCountInAtLeast = 0;

    for (int i = 1; i < cellIndices.length(); i++) {

        // Check which data type is currently to be compared and add the corresponding values
        double minTypeCount, maxTypeCount;

        switch (this->currentlyShownDataType) {
            case Helper::ShownData::RPM:
                minTypeCount = 0;
                maxTypeCount = 0;
                includeTypeCountInAtLeast = false;
                typeCountInAtLeast = 0;
                break;

            case Helper::ShownData::RAW_COUNTS:
                minTypeCount = this->minRawCount;
                maxTypeCount = this->maxRawCount;
                includeTypeCountInAtLeast = this->includeRawCountInAtLeast;
                typeCountInAtLeast = this->rawCountinAtLeast;
                break;

            case Helper::ShownData::FOLD_CHANGES:
                minTypeCount = this->minFoldChange;
                maxTypeCount = this->maxFoldChange;
                includeTypeCountInAtLeast = this->includeFoldChangeInAtLeast;
                typeCountInAtLeast = this->foldChangeInAtLeast;
                break;
        }

        // Gather the value of the currently watched cell
        double currentCellValue = this->sourceModel()->data(cellIndices.at(i)).toDouble();

        // Check whether the value lies in the boundaries of the current cut-offs
        bool isMinTypeCountCutOffMet = (currentCellValue == minTypeCount || currentCellValue > minTypeCount);
        bool isMaxTypeCountCutOffMet = (currentCellValue == maxTypeCount || currentCellValue < maxTypeCount);

        // Add increase the number of valid clusters if so
        if (isMinTypeCountCutOffMet  && isMaxTypeCountCutOffMet)
            numberOfClustersWithValidTypeCount += 1;
    }

    // ################################ IN-AT-LEAST-CUT-OFFS #############################

    if (includeTypeCountInAtLeast) {
        // If enough cluster type counts have met the required cut-off accept the row
        if (numberOfClustersWithValidTypeCount < typeCountInAtLeast)
            return false;
    } else {
        if (numberOfClustersWithValidTypeCount == 0)
            return false;
    }

    // If no filter has disqualified the row, accept it
    return true;
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
 * @brief ProxyModel::refreshData - Emit a signal for repainting the TableView
 */
void ProxyModel::refreshData() {
    QModelIndex topLeftCell = this->index(0, 0),
                bottomRightCell = this->index(this->rowCount, this->columnCount);
    emit this->dataChanged(topLeftCell, bottomRightCell, {Qt::DisplayRole});
}


/**
 * @brief ProxyModel::setCurrentlyShownDataType - Acknowledge the new data type that is currently shown
 * @param newDataTypeToShow - The new data type that has been selected to be shown in the Table View
 */
void ProxyModel::setCurrentlyShownDataType(const Helper::ShownData newDataTypeToShow) {
    this->currentlyShownDataType = newDataTypeToShow;
//    invalidateFilter();
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


/**
 * @brief ProxyModel::setSearchedGeneIDs - Sets the list of gene IDs the table is filtered by
 * @param searchedGeneIDs - List of gene IDs for filtering
 */
void ProxyModel::setSearchedGeneIDs(QStringList searchedGeneIDs) {
    if (this->searchedGeneIDs != searchedGeneIDs)
        this->searchedGeneIDs = searchedGeneIDs;
    invalidateFilter();
}
