#include "Sorter.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>
#include <functional>

#include "BioModels/Celltype.h"
#include "BioModels/Feature.h"

namespace Sorter {

/**
 * @brief sortCollectionPair
 * @param collectionA
 * @param collectionB
 * @param cmp
 */
template <typename T, typename R, typename Compare>
void sortCollectionPair(QVector<T>& collectionA, QVector<R>& collectionB, Compare comparator) {
    QVector<QPair<T,R>> pairedCollection;
    pairedCollection.reserve(collectionA.length());

    // Add each associated elements as a pair to new collection
    for (int i = 0; i < collectionA.length(); i++) {
        pairedCollection.append(qMakePair(collectionA[i], collectionB[i]));
    }

    // Sort by comparator
    qSort(pairedCollection.begin(), pairedCollection.end(), comparator);

    collectionA.clear();
    collectionB.clear();
    collectionA.reserve(pairedCollection.length());
    collectionB.reserve(pairedCollection.length());

    // Readd the sorted elements to the original collections
    for (int i = 0; i < pairedCollection.length(); i++) {
        collectionA.append(pairedCollection[i]);
        collectionB.append(pairedCollection[i]);
    }
}

/**
 * @brief rankFeaturesByExpression
 * @param featureCollection
 */
FeatureCollection sortFeaturesByExpression(FeatureCollection featureCollection) {
    //REMEMBER: This function should use less memory -> No copying of featureCollection?! -> See below
    FeatureCollection sortedCollection(featureCollection.ID);

    QVector<QPair<QString, double>> pairedCollection;
    QVector<QPair<QString, double>> pairedCollectionCopy = pairedCollection;

    // Split features into IDs and expression-counts for sorting
    for (int i = 0; i < featureCollection.getNumberOfFeatures(); i++) {
        pairedCollection.append(qMakePair(featureCollection.getFeatureID(i), featureCollection.getFeatureExpressionCount(i)));
    }

    // Use custom sorting function to sort features decreasing by expression count
    std::sort(pairedCollection.begin(), pairedCollection.end(),
              [](const QPair<QString, double> featureOne, const QPair<QString, double> featureTwo) {
                        return featureOne.second > featureTwo.second;
    });

    // Readd sorted Features to collection
    for (int i = 0; i < pairedCollection.length(); i++) {
        sortedCollection.addFeature(pairedCollection[i].first, pairedCollection[i].second);
    }

    return sortedCollection;
}

//void rankFeaturesByExpression(FeatureCollection featureCollection) {
//FeatureCollection rankFeaturesByExpression(FeatureCollection featureCollection) {
    // Copy collection that is to be sorted
//    FeatureCollection sortedCollection(featureCollection);

    // Use custom sorting function to sort features decreasing by expression count
//    std::sort(featureCollection.getFeature(0), featureCollection.getFeature(featureCollection.getNumberOfFeatures()),
//              [](const Feature featureOne, const Feature featureTwo) {
//                        return featureOne.count > featureTwo.count;
//    });

//    return featureCollection;
//}

/**
 * @brief Sorter::findHighestLikelyCellTypeMapping
 * @param clustersWithCellTypeMappingLikelihoods
 */
//void Sorter::findHighestLikelyCellTypeMapping(QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods) {
void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods) {
    qDebug() << "Start sorting.";
    double highestLikelyCellTypeMapping;
    QString cellType, tissueType;
    int i = 0;

    for (QVector<QPair<CellType, double>> cluster : clustersWithCellTypeMappingLikelihoods) {
        QStringList bla;
        CellType celltype("nAn", "nAn", bla);
        highestLikelyCellTypeMapping = 0;

        for (QPair<CellType, double> cellTypeWithMappingLikelihood : cluster) {
            bool isHigherLikelihood = cellTypeWithMappingLikelihood.second > highestLikelyCellTypeMapping;

            if (isHigherLikelihood) {
                highestLikelyCellTypeMapping = cellTypeWithMappingLikelihood.second;
                cellType = cellTypeWithMappingLikelihood.first.ID;
                tissueType = cellTypeWithMappingLikelihood.first.associatedTissueType;
            }
        }

        qDebug() << "Cluster:" << i << "is" << cellType << "/" << tissueType << "with 'likelihood' of" << highestLikelyCellTypeMapping;
        i++;
    }
}

}
