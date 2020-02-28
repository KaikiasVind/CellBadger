#include "Feature.h"

#include <QString>

Feature::Feature()
    :ID  {"nAn"}, count {-1}
{}

/**
 * @brief Feature::Feature - Container class serving as a feature - expression-count pair
 * @param ID - Feature Name (Non-standardised gene name mostly)
 * @param count - Measurement for the expression of the given feature
 */
Feature::Feature(QString featureID, double featureCount)
    : ID {featureID}, count {featureCount}
{}
