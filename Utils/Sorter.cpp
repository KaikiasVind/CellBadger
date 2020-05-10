#include "Sorter.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>
#include <functional>

#include "BioModels/Feature.h"

namespace Sorter {

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
        sortedCollection.addFeature(pairedCollection[i].first, "nAn", pairedCollection[i].second);
    }

    return sortedCollection;
}


// FIXME: This is waaaaay too slow!!
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
        featuresCollectionOne.insert(collectionOne.getFeatureEnsemblID(i), collectionOne.getFeatureExpressionCount(i));
        featureIDsCollectionOne.append(collectionOne.getFeatureEnsemblID(i));
    }
    for (int i = 0; i < numberOfFeaturesCollectionTwo; i++) {
        featuresCollectionTwo.insert(collectionTwo.getFeatureEnsemblID(i), collectionTwo.getFeatureExpressionCount(i));
        featureIDsCollectionTwo.append(collectionTwo.getFeatureEnsemblID(i));
    }

    // Comparator to sort features alphabetically by feature-ID
    auto sortFeaturesAlphabetically = [](QString featureOneEnsemblID, QString featureTwoEnsemblID) {
        return featureOneEnsemblID.compare(featureTwoEnsemblID) < 0;
    };

    // Sort the two input collections
    std::sort(featureIDsCollectionOne.begin(), featureIDsCollectionOne.end(), sortFeaturesAlphabetically);
    std::sort(featureIDsCollectionTwo.begin(), featureIDsCollectionTwo.end(), sortFeaturesAlphabetically);

    // Find intersection of the two sorted collections and fill the intersected elements into new vector
    std::set_intersection(featureIDsCollectionOne.begin(), featureIDsCollectionOne.end(), featureIDsCollectionTwo.begin(), featureIDsCollectionTwo.end(), std::back_inserter(intersectedCollection));

    equallyExpressedFeatures.reserve(intersectedCollection.length());

    // Add all intersected features from original collections as pair into result vector
    for (QString featureEnsemblID : intersectedCollection) {
        // Retrieve feature expression counts from Feature map
        double collectionOneExpressionCount = featuresCollectionOne[featureEnsemblID],
               collectionTwoExpressionCount = featuresCollectionTwo[featureEnsemblID];

        // Reassamble Features with found feature expression counts from map and feature ID from intersection
        Feature featureCollectionOne(featureEnsemblID, "nAn", collectionOneExpressionCount),
                featureCollectionTwo(featureEnsemblID, "nAn", collectionTwoExpressionCount);

        // Add these as pair to result vector
        equallyExpressedFeatures.append(qMakePair(featureCollectionOne, featureCollectionTwo));
    }

    return equallyExpressedFeatures;
}

}
