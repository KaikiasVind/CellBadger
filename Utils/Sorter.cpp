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

// This is too waaaaay fucking slow.
/**
 * @brief findEquallyExpressedFeatures
 * @param collectionOne
 * @param collectionTwo
 * @return
 */
QVector<QPair<Feature, Feature>> findEquallyExpressedFeatures(FeatureCollection collectionOne, FeatureCollection collectionTwo) {
    QVector<QPair<Feature, Feature>> equallyExpressedFeatures;

    int numberOfFeaturesCollectionOne = collectionOne.getNumberOfFeatures(),
        numberOfFeaturesCollectionTwo = collectionTwo.getNumberOfFeatures();

    // Create vector of QStrings to easily create intersection
    QVector<QString> featureIDsCollectionOne,
                     featureIDsCollectionTwo,
                     // This vector is used to store the intersected elements
                     intersectedCollection;
    featureIDsCollectionOne.reserve(numberOfFeaturesCollectionOne);
    featureIDsCollectionTwo.reserve(numberOfFeaturesCollectionTwo);

    // Create hash-map to easily look up Features
    QHash<QString, double> featuresCollectionOne,
                           featuresCollectionTwo;
    featuresCollectionOne.reserve(numberOfFeaturesCollectionOne);
    featuresCollectionTwo.reserve(numberOfFeaturesCollectionTwo);

    // Fill hash-maps
    for (int i = 0; i < numberOfFeaturesCollectionOne; i++) {
        featuresCollectionOne.insert(collectionOne.getFeatureID(i), collectionOne.getFeatureExpressionCount(i));
        featureIDsCollectionOne.append(collectionOne.getFeatureID(i));
    }
    for (int i = 0; i < numberOfFeaturesCollectionTwo; i++) {
        featuresCollectionTwo.insert(collectionTwo.getFeatureID(i), collectionTwo.getFeatureExpressionCount(i));
        featureIDsCollectionTwo.append(collectionTwo.getFeatureID(i));
    }

    // Comparator to sort features alphabetically by feature-ID
    auto sortFeaturesAlphabetically = [](QString featureOneID, QString featureTwoID) {
        return featureOneID.compare(featureTwoID) < 0;
    };

    // Sort the two input collections
    std::sort(featureIDsCollectionOne.begin(), featureIDsCollectionOne.end(), sortFeaturesAlphabetically);
    std::sort(featureIDsCollectionTwo.begin(), featureIDsCollectionTwo.end(), sortFeaturesAlphabetically);

    // Find intersection of the two sorted collections and fill the intersected elements into new vector
    std::set_intersection(featureIDsCollectionOne.begin(), featureIDsCollectionOne.end(), featureIDsCollectionTwo.begin(), featureIDsCollectionTwo.end(), std::back_inserter(intersectedCollection));

    equallyExpressedFeatures.reserve(intersectedCollection.length());

    // Add all intersected features from original collections as pair into result vector
    for (QString featureID : intersectedCollection) {
        // Retrieve feature expression counts from Feature map
        double collectionOneExpressionCount = featuresCollectionOne[featureID],
               collectionTwoExpressionCount = featuresCollectionTwo[featureID];

        // Reassamble Features with found feature expression counts from map and feature ID from intersection
        Feature featureCollectionOne(featureID, collectionOneExpressionCount),
                featureCollectionTwo(featureID, collectionTwoExpressionCount);

        // Add these as pair to result vector
        equallyExpressedFeatures.append(qMakePair(featureCollectionOne, featureCollectionTwo));
    }

//    for (int i = 0; i < collectionOne.getNumberOfFeatures(); i++) {
//        Feature currentFeature = collectionOne.getFeature(i);
//        bool isFeatureExpressedEqually = collectionTwo.isFeatureExpressed(currentFeature);

//        if (isFeatureExpressedEqually) {
//            equallyExpressedFeatures.append(qMakePair(currentFeature, collectionTwo.getFeature(currentFeature.ID)));
//        }
//    }


    // ############################################# REMAINS #############################################
//    for (Feature feature : featuresCollectionOne) {
//        Feature * addressFoundFeature = std::find_if(featuresCollectionTwo.begin(), featuresCollectionTwo.end(),
//                     [feature](Feature currentFeature) -> bool {
//                return currentFeature.ID == feature.ID;
//        });
//        if (addressFoundFeature != nullptr) {
//            Feature foundFeature = * addressFoundFeature;
//            qDebug() << "Found feature:" << addressFoundFeature->ID;
//            equallyExpressedFeatures.append(qMakePair(feature, foundFeature));
//        }
//    }
//    qDebug() << "Done";

//    auto comparator = [](Feature featureOne, Feature featureTwo) { return featureOne.ID == featureTwo.ID; };

//    for (QPair<Feature, Feature> pair : equallyExpressedFeatures)
//        qDebug() << pair.first.ID << ":" << pair.first.count << "-" << pair.second.ID << ":" << pair.second.count;
    return equallyExpressedFeatures;
}

}
