#ifndef SORTER_H
#define SORTER_H

#include <QVector>

#include "BioModels/FeatureCollection.h"

/**
 * @brief The Sorter struct is a utility class for everything concerned with sorting
 */
namespace Sorter
{
    extern QVector<int> calculateRanks(QVector<double> numbers);

    extern QVector<QPair<Feature, Feature>> findEquallyExpressedFeatures(FeatureCollection collectionOne, FeatureCollection collecionTwo);
};

#endif // SORTER_H
