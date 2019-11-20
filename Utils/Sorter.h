#ifndef SORTER_H
#define SORTER_H

#include <QVector>

#include "BioModels/Celltype.h"
#include "BioModels/FeatureCollection.h"

/**
 * @brief The Sorter struct is a utility class for everything concerned with sorting
 */
namespace Sorter
{
//    extern void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods);
    extern void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods);

//    extern void rankFeaturesByExpression(FeatureCollection& featureCollection);
    extern FeatureCollection rankFeaturesByExpression(FeatureCollection featureCollection);

    template <typename T, typename R, typename Compare>
    extern void sortCollectionPair(QVector<T>& collectionA, QVector<R>& collectionB, Compare cmp);
};

#endif // SORTER_H
