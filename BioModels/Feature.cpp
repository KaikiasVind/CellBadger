#include "Feature.h"

#include <QString>

Feature::Feature()
    : ID {"nAn"}, count {-1.}, log2FoldChange {-1.}, foldChange {-1.}, sensitivity {-1.}, specifity {-1.}
{}

/**
 * @brief Feature::Feature - Container class serving as a feature - expression-count pair
 * @param ID - Feature Name (Non-standardised gene name mostly)
 * @param count - Measurement for the expression of the given feature
 */
Feature::Feature(const QString ID, const double meanCount)
    : ID {ID}, count {meanCount}, log2FoldChange {-1.}, foldChange {-1.}, sensitivity {-1.}, specifity {-1.}
{}


/**
 * @brief Feature::Feature
 * @param featureID
 * @param featureCount
 * @param log2FoldChange
 * @param foldChange
 */
Feature::Feature(const QString ID, const double meanCount, const double log2FoldChange, const double foldChange)
    : ID {ID}, count {meanCount}, log2FoldChange {log2FoldChange}, foldChange {foldChange}, sensitivity {-1.}, specifity {-1.}
{}


/**
 * @brief Feature
 * @param ID - Gene identifier
 * @param associatedTissue - tissue that
 * @param sensitivity - Sensitivity for the cell-type this gene belongs to
 * @param specifity - Specifity for the cell-type this gene belongs to
 */
Feature::Feature(const QString ID, const double sensitivity, const double specifity)
    : ID {ID}, sensitivity {sensitivity}, specifity {specifity}, count {-1.}, log2FoldChange {-1.}, foldChange {-1.}
{}
