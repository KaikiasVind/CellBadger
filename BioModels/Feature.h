#ifndef FEATURE_H
#define FEATURE_H

#include <QString>

/**
 * @brief The Feature struct serves as a container representing a feature - expression-count pair
 */
struct Feature
{
    QString ID;
    QString ensemblID;
    double count;
    double log2FoldChange;
    double foldChange;

    Feature();
    Feature(const QString ID);
    Feature(const QString ID, const QString ensemblID, const double meanCount, const double log2FoldChange, const double foldChange);
    Feature(const QString ID, const QString ensemblID, const double log2FoldChange, const double foldChange);
};

#endif // FEATURE_H
