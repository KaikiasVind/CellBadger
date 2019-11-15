#ifndef CLUSTER_H
#define CLUSTER_H

#include <QStringList>
#include <QVector>

#include "BioModels/Feature.h"

/**
 * @brief The Cluster class serves as a container class for features
 */
class Cluster
{
private:
    QVector<Feature> features;

public:
    Cluster();

    void addFeature(QString featureID, double expressionCount);

    bool isMarkerExpressed(QString markerID);

    Feature getFeature(int index);
    QString getFeatureID(int index);
    double getFeatureExpressionCount(int index);

};

#endif // CLUSTER_H
