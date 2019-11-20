#include "FeatureCollection.h"

#include <QStringList>
#include <QVector>
#include <QDebug>

#include "BioModels/Feature.h"

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
 * @brief Cluster::addFeature
 * @param featureID
 * @param expressionCount
 */
void FeatureCollection::addFeature(QString featureID, double expressionCount) {
    Feature feature(featureID, expressionCount);
    features.append(feature);
}

/**
 * @brief FeatureCollection::addFeature
 * @param feature
 */
void FeatureCollection::addFeature(Feature feature) {
    features.append(feature);
}

bool FeatureCollection::isFeatureExpressed(QString markerID) {
    //REMEMBER: MAYBE -> WOULDTHAT WORK?
//    Feature feature(markerID, COUNT);
//    return features.contains(feature);
    //REMEMBER: MAYBE -> WOULDTHAT WORK?
    for (Feature feature : features) {
        bool isSameFeature = feature.ID == markerID;
        if (isSameFeature)
            return true;
    }
    return false;
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
    return features;
}
