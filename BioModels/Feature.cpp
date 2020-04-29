#include "Feature.h"

#include <QString>

Feature::Feature() {};

/**
 * @brief Feature::Feature - Container class serving as a feature - expression-count pair
 * @param ID - Feature Name (Non-standardised gene name mostly)
 * @param count - Measurement for the expression of the given feature
 */
Feature::Feature(QString featureID, QString ensemblID, double featureCount)
    : ID {featureID}, ensemblID(ensemblID), count {featureCount}
{}
