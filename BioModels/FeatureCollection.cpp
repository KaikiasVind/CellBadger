#include "FeatureCollection.h"

#include <QStringList>
#include <QVector>
#include <math.h>
#include <QDebug>

#include "BioModels/Feature.h"

FeatureCollection::FeatureCollection(){}

/**
 * @brief Cluster::Cluster - Only container class - only default constructor
 */
FeatureCollection::FeatureCollection(QString collectionID)
    : ID (collectionID)
{}

//FeatureCollection::FeatureCollection(FeatureCollection & featureCollection)
//    : ID (featureCollection.ID)
//{
//    features.reserve(featureCollection.getNumberOfFeatures());
//    for (int i = 0; i < featureCollection.getNumberOfFeatures(); i++)
//        features.append(featureCollection.getFeature(i));
//}

/**
 * @brief FeatureCollection::addFeature
 * @param feature
 */
void FeatureCollection::addFeature(const Feature feature) {
    this->features.append(feature);
}


// REMEMBER: Do I need this one?
/**
 * @brief FeatureCollection::addFeature - The FeatureCollection that is appended to here represents a 10x Cluster and the new Feature represents an expressed gene in that cluster
 * @param featureID - Gene ID
 * @param featureMeanCount - Mean count of the expression of the selected gene / UMI count for the selected gene
 */
void FeatureCollection::addFeature(const QString featureID, const double featureMeanCount) {
    Feature feature(featureID, featureMeanCount);
    this->features.append(feature);
}


/**
 * @brief FeatureCollection::addFeature - The FeatureCollection that is appended to here represents a cell type and the new Feature represents a gene / cell marker
 * @param ID - Gene ID
 * @param sensitivity - Gene expression sensitivity for the cell type it is appended to
 * @param specifity - Gene expression specifity for the cell type it is appended to
 */
void FeatureCollection::addFeature(const QString featureID, const double featureSensitivity, const double featureSpecifity) {
    Feature feature(featureID, featureSensitivity, featureSpecifity);
    feature.foldChange = featureSensitivity / featureSpecifity;
    feature.log2FoldChange = log2(feature.foldChange);
    this->features.append(feature);
}


/**
 * @brief FeatureCollection::addFeature - The FeatureCollection that is appended to here represents a 10x Cluster and the new Feature represents an expressed gene in that cluster
 * @param featureID - Gene ID
 * @param featureMeanCount - Mean count of the expression of the selected gene / UMI count for the selected gene
 * @param featureLog2FoldChange - Log 2 of ratio of expression of the selected gene in comparison to the expression of this gene in every other cluster
 * @param featureFoldChange - Actual ratio of expression of the selected gene in comparison to the expression of this gene in every other cluster
 */
void FeatureCollection::addFeature(const QString featureID, const double featureMeanCount, const double featureLog2FoldChange, const double featureFoldChange) {
    Feature feature(featureID, featureMeanCount, featureLog2FoldChange, featureFoldChange);
    this->features.append(feature);
}


/**
 * @brief FeatureCollection::filterFeatures - Keep only $number most expressed features
 * @param number - Number of features to keep
 */
QVector<double> FeatureCollection::getMostExpressedFeaturesCounts(int number) {
    QVector<double> sortedFeaturesExpressions;
    sortedFeaturesExpressions.reserve(this->getNumberOfFeatures());
    for (int i = 0; i < this->getNumberOfFeatures(); i++) {
        sortedFeaturesExpressions.append(this->getFeatureExpressionCount(i));
    }
    std::sort(sortedFeaturesExpressions.begin(), sortedFeaturesExpressions.end());
    sortedFeaturesExpressions.resize(number);
    sortedFeaturesExpressions.squeeze();
    return sortedFeaturesExpressions;
}

bool FeatureCollection::isFeatureExpressed(QString markerID) {
    //REMEMBER: MAYBE -> WOULDTHAT WORK?
//    Feature feature(markerID, COUNT);
//    return features.contains(feature);
    for (Feature feature : this->features) {
        bool isSameFeature = feature.ID == markerID;
        if (isSameFeature)
            return true;
    }
    return false;
}

bool FeatureCollection::isFeatureExpressed(Feature feature) {
    return this->isFeatureExpressed(feature.ID);
}

/**
 * @brief Cluster::getFeature
 * @param index
 * @return
 */
Feature FeatureCollection::getFeature(int index) {
    return features[index];
}

/**
 * @brief FeatureCollection::getFeature
 * @param ID
 * @return
 */
Feature FeatureCollection::getFeature(QString featureID) {
    for (Feature feature : this->features) {
        if (feature.ID == featureID)
            return feature;
    }
    Feature noFeature("nAn", -1., -1., -1.);
    return noFeature;
}

/**
 * @brief Cluster::getFeatureID
 * @param index
 * @return
 */
QString FeatureCollection::getFeatureID(int index) {
    return features[index].ID;
}

/**
 * @brief Cluster::getFeatureExpressionCount
 * @param index
 * @return
 */
double FeatureCollection::getFeatureExpressionCount(int index) {
    return features[index].count;
}

/**
 * @brief FeatureCollection::getNumberOfFeatures
 * @return Number of expressed features
 */
int FeatureCollection::getNumberOfFeatures() {
    return features.length();
}

/**
 * @brief FeatureCollection::getFeatures
 * @return List of features associated with FeatureCollection
 */
QVector<Feature> FeatureCollection::getFeatures() {
    QVector<Feature> copyCollection = this->features;
    return copyCollection;
}


/**
 * @brief FeatureCollection::getFoldChangeSum - Calculates the fold change sum for the current genes
 * @return - Sum of all genes currently listed in this FeatureCollection
 */
double FeatureCollection::getFoldChangeSum() {
    double foldChangeSum = 0;
    for (Feature feature : this->features) {
        foldChangeSum += feature.foldChange;
    }
    return foldChangeSum;
}
