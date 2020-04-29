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

    Feature();
    Feature(const QString ID, const QString ensemblID, const double count);
};

#endif // FEATURE_H
