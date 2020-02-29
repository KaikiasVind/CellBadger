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

    Feature();
    Feature(const QString ID, const double count, double log2FoldChange, double foldChange);
};

#endif // FEATURE_H
