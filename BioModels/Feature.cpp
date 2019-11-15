#include "Feature.h"

#include <QString>

/**
 * @brief Feature::Feature - Container class serving as a feature - expression-count pair
 * @param ID - Feature Name (Non-standardised gene name mostly)
 * @param count - Measurement for the expression of the given feature
 */
Feature::Feature(QString ID, double count)
    : ID {ID}, count {count}
{}
