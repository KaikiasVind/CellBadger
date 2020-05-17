#ifndef SORTER_H
#define SORTER_H

#include <QVector>

#include "BioModels/FeatureCollection.h"

/**
 * @brief The Sorter namespace combines everything concerned with sorting
 */
namespace Sorter
{
    extern void sortCellTypeFoldChangeSumsAfterDistanceToClusterFoldChangeSums(QVector<QVector<QPair<QString, QPair<double, double>>>> & cellTypeFoldChangeSumsFor10xClusters);
};

#endif // SORTER_H
