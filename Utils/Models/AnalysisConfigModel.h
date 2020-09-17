#ifndef ANALYSISCONFIGMODEL_H
#define ANALYSISCONFIGMODEL_H

#include "Utils/Helper.h"

using Helper::AnalysisFilterMode;

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
    AnalysisConfigModel(const AnalysisFilterMode usedAnalysisFilterMode);
};

#endif // ANALYSISCONFIGMODEL_H
