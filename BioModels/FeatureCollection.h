#ifndef CLUSTER_H
#define CLUSTER_H

#include <QStringList>
#include <QVector>

#include "BioModels/Feature.h"

/**
 * @brief The FeatureCollection class serves as a container class for features. It is one of 10x-cluster or tissue-type
 */
class FeatureCollection
{
private:
    QVector<Feature> features;

public:
    QString ID;

    FeatureCollection(QString collectionID);
//    FeatureCollection(FeatureCollection & featureCollection);

    void addFeature(QString featureID, double expressionCount);
    void addFeature(Feature feature);

    bool isFeatureExpressed(QString markerID);

    Feature getFeature(int index);
    QString getFeatureID(int index);
    double getFeatureExpressionCount(int index);
    int getNumberOfFeatures();
    QVector<Feature> getFeatures();

};

#endif // CLUSTER_H
