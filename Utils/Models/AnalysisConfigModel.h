#ifndef ANALYSISCONFIGMODEL_H
#define ANALYSISCONFIGMODEL_H

#include "Utils/Definitions.h"

using Definitions::AnalysisFilterMode;

struct AnalysisConfigModel {

    // BASIC CONFIGURATION
    AnalysisFilterMode usedFilterMode;

    // TOP_N FILTER MODE
    int numberOfGenesToUse;

    // MANUAL FILTER MODE
    // Raw Count
    double minRawCount;
    double maxRawCount;
    bool includeRawCountInAtLeast;
    int rawCountInAtLeast;

    // Fold change
    double minFoldChange;
    double maxFoldChange;
    bool includeFoldChangeInAtLeast;
    int foldChangeInAtLeast;

    AnalysisConfigModel();
    AnalysisConfigModel(const AnalysisFilterMode analysisFilterMode);
};

#endif // ANALYSISCONFIGMODEL_H
