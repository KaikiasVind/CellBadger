#include "Cluster.h"

#include <QStringList>
#include <QVector>

#include "BioModels/Feature.h"

/**
 * @brief Cluster::Cluster - Only container class - only default constructor
 */
Cluster::Cluster(){}

/**
 * @brief Cluster::addFeature
 * @param featureID
 * @param expressionCount
 */
void Cluster::addFeature(QString featureID, double expressionCount) {
    Feature feature(featureID, expressionCount);
    features.append(feature);
}

bool Cluster::isMarkerExpressed(QString markerID) {
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
Feature Cluster::getFeature(int index) {
    return features[index];
}

/**
 * @brief Cluster::getFeatureID
 * @param index
 * @return
 */
QString Cluster::getFeatureID(int index) {
    return features[index].ID;
}

/**
 * @brief Cluster::getFeatureExpressionCount
 * @param index
 * @return
 */
double Cluster::getFeatureExpressionCount(int index) {
    return features[index].count;
}
