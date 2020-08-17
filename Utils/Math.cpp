#include "Math.h"

#include <math.h>

#include "BioModels/FeatureCollection.h"

namespace Math {

// ####################################### OPERATIONS #######################################
template <typename N, typename R>
/**
 * @brief mean
 * @param numbers
 * @return
 */
R mean(QVector<N> numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), .0) / numbers.length();
}

/**
 * @brief mean
 * @param numbers
 * @return
 */
double mean(QVector<double> numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), .0) / numbers.length();
}

/**
 * @brief mean
 * @param numbers
 * @return
 */
double mean(QVector<int> numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), .0) / numbers.length();
}

/**
 * @brief invertLog - Takes a number x and a base number b with log_b(y) = x and returns y (with b € N, x,y € R)
 * @param base - Base for log function
 * @param realNumber - Number x that was the result of log_b(y) = x
 * @return - Inverts the log function and presents y = b^x
 */
double invertLog(int base, double realNumber) {
    return pow(base, realNumber);
}


// ####################################### SPECIFICS #######################################
/**
 * @brief calculateQualityScores - Calculates the quality scores for all given correlations
 * @param correlations - List of cluster-cell-type correlations
 * @return - List of quality scores for cluster-cell-type correlations
 */
QVector<double> calculateQualityScores(QVector<QVector<QPair<QString, double>>> correlations) {
    QVector<double> qualityScores;
    qualityScores.reserve(correlations.length());

    // Return the subtraction of the second correlation value from the first
    std::transform(correlations.begin(), correlations.end(), std::back_inserter(qualityScores),
                   [](QVector<QPair<QString, double>> correlation) {
        return correlation.at(0).second - correlation.at(1).second;
    });

    return qualityScores;
}


/**
 * @brief calculateManRawCountForGene - Calculate the mean of the expression counts of the given gene in all clusters
 * @param geneID - Name of the given gene
 * @param clusters - All clusters maybe expressing the gene
 * @return - Mean of all found expression counts of the given gene in all given clusters
 */
double calculateMeanRawCountForGene(QString geneID, QVector<FeatureCollection> clusters) {
    double meanRawCount = 0;
    for (FeatureCollection cluster : clusters) {
        double rawCountForCurrentGene = cluster.getFeature(geneID).count;

        if (rawCountForCurrentGene != -1)
            meanRawCount += rawCountForCurrentGene;
    }

    return meanRawCount / clusters.length();
}


}


