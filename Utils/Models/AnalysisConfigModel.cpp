#include "AnalysisConfigModel.h"

#include "Utils/Helper.h"

using Helper::AnalysisFilterMode;

/**
 * @brief AnalysisConfigModel::AnalysisConfigModel - Default constructor
 */
AnalysisConfigModel::AnalysisConfigModel() {};

/**
 * @brief AnalysisConfigModel::AnalysisConfigModel - A single model that combines all chose filter config values
 * @param usedAnalysisFilterMode - Mode that is chosen for analysis
 */
AnalysisConfigModel::AnalysisConfigModel(const AnalysisFilterMode usedAnalysisFilterMode)
    : usedFilterMode(usedAnalysisFilterMode)
{};
