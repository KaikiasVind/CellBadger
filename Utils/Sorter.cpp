#include "Sorter.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>
#include <functional>

#include "BioModels/Feature.h"

namespace Sorter {

/**
 * @brief sortCellTypeFoldChangeSumsAfterDistanceToClusterFoldChangeSums - Sorts the fold change sums for every cell type acording to the minimal distance to the clusters fold change sum - ATTENTION WORKS IN PLACE!
 * @param cellTypeFoldChangeSumsFor10xClusters - List of cell types with fold change sum and distance to fold change sum of cluster
 */
void sortCellTypeFoldChangeSumsAfterDistanceToClusterFoldChangeSums(QVector<QVector<QPair<QString, QPair<double, double>>>> & cellTypeFoldChangeSumsFor10xClusters) {
    for (int i = 0; i < cellTypeFoldChangeSumsFor10xClusters.length(); i++) {
        std::sort(cellTypeFoldChangeSumsFor10xClusters[i].begin(), cellTypeFoldChangeSumsFor10xClusters[i].end(),
            [](QPair<QString, QPair<double, double>> cellTypeOne, QPair<QString, QPair<double, double>> cellTypeTwo) {
                return (cellTypeOne.second.second < cellTypeTwo.second.second);
        });
    }
}

}
