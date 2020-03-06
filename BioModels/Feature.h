#ifndef FEATURE_H
#define FEATURE_H

#include <QString>

/**
 * @brief The Feature struct serves as a container representing a feature - expression-count pair
 */
struct Feature
{
    QString ID;
    double count;
    double log2FoldChange;
    double foldChange;
    double sensitivity;
    double specifity;

    Feature();
    Feature(const QString ID, const double meanCount);
    Feature(const QString ID, const double meanCount, const double log2FoldChange, const double foldChange);
    Feature(const QString ID, const double sensitivity, const double specifity);
};

#endif // FEATURE_H
