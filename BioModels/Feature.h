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

    Feature(QString ID, double count);
};

#endif // FEATURE_H
