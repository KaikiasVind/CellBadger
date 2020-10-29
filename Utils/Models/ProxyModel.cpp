#include "ProxyModel.h"

#include <QSortFilterProxyModel>
#include <QDebug>

#include "Utils/Definitions.h"

using Definitions::ShownData;

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
ProxyModel::ProxyModel(int rowCount, int colCount, double maxRawCount, double maxFoldChange, QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashedGeneDataForAllClusters, QObject * parent) : QSortFilterProxyModel(parent),
    rowCount (rowCount), columnCount (colCount), minRawCount (0.0), maxRawCount (maxRawCount), hashedGeneDataForAllClusters(hashedGeneDataForAllClusters), minFoldChange(0.0), maxFoldChange(maxFoldChange),
  rawCountinAtLeast(0), includeRawCountInAtLeast(false),foldChangeInAtLeast(0), includeFoldChangeInAtLeast(false),
  searchedGeneIDs(QStringList()) {};


/**
 * @brief ProxyModel::filterAcceptsRow - Evaluates whether the selected row meets the given requirements
 * @param source_row - The selected row
 * @param source_parent - The parent widget
 * @return - Returns whether the selected row meets the given requirements
 */
bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {
    Q_UNUSED(source_parent);

    // Grab the gene ID for the current row
    QModelIndex currentRowGeneIDModelIndex = this->sourceModel()->index(source_row, 0);
    QString currentRowGeneID = this->sourceModel()->data(currentRowGeneIDModelIndex).toString();

    // ################################ GENE-IDS ################################
    // If there are gene IDs the table should be filtered by check the selected rows for those gene IDs
    if (!this->searchedGeneIDs.isEmpty()) {

        // Go through the list of given gene IDs and search for the current row's gene ID
        // Accept the row if one has been found
        QStringList unseenGeneIDs = this->searchedGeneIDs;
        bool isIDHasBeenSeen = false;
        for (int i = 0; i < unseenGeneIDs.length(); i++) {
            if (currentRowGeneID.toLower().contains(this->searchedGeneIDs.at(i))) {
                isIDHasBeenSeen = true;
            }
        }
        if (!isIDHasBeenSeen)
            return false;
    }

    // ################################ CUT-OFFS ################################

    int numberOfClustersWithValidRPMValue = 0, numberOfClustersWithValidRawCount = 0, numberOfClustersWithValidFoldChange = 0;

    // Search for the geneID of the current row in the hash map
    std::tuple<QVector<double>, QVector<double>, QVector<double>> currentlyWatchedGeneData = this->hashedGeneDataForAllClusters[currentRowGeneID];

    // Go through the list of gene expression data of the current gene and compare it against the cut-offs
    for (int i = 0; i < this->columnCount - 1; i++) {
        double currentlyWatchedGeneRPMValue = std::get<0>(currentlyWatchedGeneData).at(i),
                currentlyWatchedGeneRawCount = std::get<1>(currentlyWatchedGeneData).at(i),
                currentlyWatchedGeneFoldChange = std::get<2>(currentlyWatchedGeneData).at(i);

        // Check whether the value lies in the boundaries of the current cut-offs
        // Add increase the number of valid clusters if so
        // ########################################## RMP VALUES ##########################################
        bool isMinRPMValueCutOffMet = (currentlyWatchedGeneRPMValue == 0 || currentlyWatchedGeneRawCount > 0);
        bool isMaxRPMValueCutOffMet = (currentlyWatchedGeneRPMValue == 0 || currentlyWatchedGeneRawCount < 0);

        if (isMinRPMValueCutOffMet && isMaxRPMValueCutOffMet)
             numberOfClustersWithValidRPMValue += 1;

        // ########################################## RAW COUNTS ##########################################
        bool isMinRawCountCutOffMet = (currentlyWatchedGeneRawCount == this->minRawCount || currentlyWatchedGeneRawCount > this->minRawCount);
        bool isMaxRawCountCutOffMet = (currentlyWatchedGeneRawCount == this->maxRawCount || currentlyWatchedGeneRawCount < this->maxRawCount);

        if (isMinRawCountCutOffMet && isMaxRawCountCutOffMet)
             numberOfClustersWithValidRawCount += 1;

        // ########################################## FOLD CHANGES ########################################
        bool isMinFoldChangeCutOffMet = (currentlyWatchedGeneFoldChange == this->minFoldChange || currentlyWatchedGeneFoldChange > this->minFoldChange);
        bool isMaxFoldChangeCutOffMet = (currentlyWatchedGeneFoldChange == this->maxFoldChange || currentlyWatchedGeneFoldChange < this->maxFoldChange);

        if (isMinFoldChangeCutOffMet && isMaxFoldChangeCutOffMet)
             numberOfClustersWithValidFoldChange += 1;
    }

    // ################################ IN-AT-LEAST-CUT-OFFS #############################

    // ################################## RAW COUNTS ##################################
    if (this->includeRawCountInAtLeast) {
        if (numberOfClustersWithValidRawCount < this->rawCountinAtLeast)
            return false;
    } else {
        if (numberOfClustersWithValidRawCount == 0)
            return false;
    }

    // ################################## FOLD CHANGES ################################
    if (this->includeFoldChangeInAtLeast) {
        if (numberOfClustersWithValidFoldChange < this->foldChangeInAtLeast)
            return false;
    } else {
        if (numberOfClustersWithValidFoldChange == 0)
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
 * @brief ProxyModel::setCurrentlyShownDataType - Acknowledge the new data type that is currently shown
 * @param newDataTypeToShow - The new data type that has been selected to be shown in the Table View
 */
void ProxyModel::setCurrentlyShownDataType(const ShownData newDataTypeToShow) {
    this->currentlyShownDataType = newDataTypeToShow;
    invalidateFilter();
}


/**
 * @brief ProxyModel::setSearchedGeneIDs - Sets the list of gene IDs the table is filtered by
 * @param searchedGeneIDs - List of gene IDs for filtering
 */
void ProxyModel::setSearchedGeneIDs(const QStringList searchedGeneIDs) {
    if (this->searchedGeneIDs != searchedGeneIDs)
        this->searchedGeneIDs = searchedGeneIDs;
    invalidateFilter();
}

// #################################### RPM ####################################

/**
 * @brief ProxyModel::setMinRPM - Set the min accepted RPM value
 * @param minRPM - RPM value
 */
void ProxyModel::setMinRPM(const double minRPM) {
    if (this->minRPM != minRPM)
        this->minRPM = minRPM;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setMaxRPM - Set the max accepted RPM value
 * @param maxRPM - RPM value
 */
void ProxyModel::setMaxRPM(const double maxRPM) {
    if (this->maxRPM != maxRPM)
        this->maxRPM = maxRPM;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setRPMInAtLeast - Sets the minimum number of cluster that should met the RPM cut-offs
 * @param amount - Minimum number of clusters
 */
void ProxyModel::setRPMInAtLeast(const int amount) {
    if (this->rpmInAtLeast != amount)
        this->rpmInAtLeast = amount;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setIncludeRPMInAtLeast - Sets whether the minimum amount of cluster sould be considered or ignorded (In case the gui element is deactivated)
 * @param state - Whether or not the number should be considered or ignored
 */
void ProxyModel::setIncludeRPMInAtLeast(const bool state) {
    if (this->includeRPMInAtLeast != state)
        this->includeRPMInAtLeast = state;
    invalidateFilter();
}

// ################################# RAW COUNT #################################

/**
 * @brief ProxyModel::setMinRawCount - Sets the min accepted raw count
 * @param minRawCount - Raw Count
 */
void ProxyModel::setMinRawCount(const double minRawCount) {
    if (this->minRawCount != minRawCount)
        this->minRawCount = minRawCount;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setMaxRawCount - Set the max accepted raw count
 * @param maxRawCoun - Raw Countt
 */
void ProxyModel::setMaxRawCount(const double maxRawCount) {
    if (this->maxRawCount != maxRawCount)
        this->maxRawCount = maxRawCount;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setRawCountInAtLeast - Sets the minimum number of cluster that should met the raw-count cut-offs
 * @param amount - Minimum number of clusters
 */
void ProxyModel::setRawCountInAtLeast(const int amount) {
    if (this->rawCountinAtLeast != amount)
        this->rawCountinAtLeast = amount;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setIncludeRawCountInAtLeast - Sets whether the minimum amount of cluster sould be considered or ignorded (In case the gui element is deactivated)
 * @param state - Whether or not the number should be considered or ignored
 */
void ProxyModel::setIncludeRawCountInAtLeast(const bool state) {
    if (this->includeRawCountInAtLeast != state)
        this->includeRawCountInAtLeast = state;
    invalidateFilter();
}


// ################################# FOLD CHANGE: #################################

/**
 * @brief ProxyModel::setMinFoldChange - Set the min accepted fold change
 * @param minFoldChange - Fold Change
 */
void ProxyModel::setMinFoldChange(const double minFoldChange) {
    if (this->minFoldChange != minFoldChange)
        this->minFoldChange = minFoldChange;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setMaxFoldChange - Set the max accepted fold change
 * @param maxFoldChange - Fold Change
 */
void ProxyModel::setMaxFoldChange(const double maxFoldChange) {
    if (this->maxFoldChange != maxFoldChange)
        this->maxFoldChange = maxFoldChange;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setFoldChangeInAtLeast - Sets the minimum number of cluster that should met the fold-change cut-offs
 * @param amount - Minimum number of clusters
 */
void ProxyModel::setFoldChangeInAtLeast(const int amount) {
    if (this->foldChangeInAtLeast != amount)
        this->foldChangeInAtLeast = amount;
    invalidateFilter();
}

/**
 * @brief ProxyModel::setIncludeFoldChangeInAtLeast - Sets whether the minimum amount of cluster sould be considered or ignorded (In case the gui element is deactivated)
 * @param state - Whether or not the number should be considered or ignored
 */
void ProxyModel::setIncludeFoldChangeInAtLeast(const bool state) {
    if (this->includeFoldChangeInAtLeast != state)
        this->includeFoldChangeInAtLeast = state;
    invalidateFilter();
}


