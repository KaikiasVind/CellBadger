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
    extern void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods);

    extern FeatureCollection sortFeaturesByExpression(FeatureCollection featureCollection);

    template <typename T, typename R, typename Compare>
    extern void sortCollectionPair(QVector<T>& collectionA, QVector<R>& collectionB, Compare cmp);

    extern QVector<int> calculateRanks(QVector<double> numbers);

    extern QVector<QPair<Feature, Feature>> findEquallyExpressedFeatures(FeatureCollection collectionOne, FeatureCollection collecionTwo);
};

#endif // SORTER_H
