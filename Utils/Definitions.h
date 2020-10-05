#ifndef DEFINITIONS_H
#define DEFINITIONS_H

namespace Definitions {

/**
 * @brief The AnalysisFilterMode enum - The filter mode that is used to filter the clusters before correlation
 */
enum class AnalysisFilterMode {
    TOP_N,
    MANUAL,
    AUTOMATIC
};

/**
 * @brief The ShownData enum - The data type that is shown in the Main Window's gene expression table
 */
enum class ShownData {
    RPM,
    RAW_COUNTS,
    FOLD_CHANGES
};

}

#endif // DEFINITIONS_H
