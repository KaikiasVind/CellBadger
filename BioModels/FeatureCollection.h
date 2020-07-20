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

    double expressionCountSum;
    double highestRawCount;
    double highestFoldChange;

public:
    QString ID;

    FeatureCollection();
    FeatureCollection(const QString collectionID);

    void addFeature(const Feature feature);
    void addFeature(const QString featureID);
    void addFeature(const QString featureID, const QString featureEnsemblID, const double featureSensitivity, const double featureSpecifity);
    void addFeature(const QString featureID, const QString featureEnsemblID, const double featureMeanCount, const double featureLog2FoldChange, const double featureFoldChange);

    bool isFeatureExpressed(QString markerID);
    bool isFeatureExpressed(Feature feature);

    Feature getFeature(int index) const;
    Feature getFeature(QString featureName) const;
    QVector<Feature> getFeatures() const;

    int getNumberOfFeatures() const;

    QString getFeatureID(int index) const;
    double getFeatureExpressionCount(int index) const;
    double getFeatureFoldChange(int index) const;
    double getFeatureRawCount(int index) const;
    double getFoldChangeSum() const;
    double getExpressionCountSum() const;
    double getHighestRawCount() const;
    double getHighestFoldChange() const;
};

#endif // CLUSTER_H
