#include "Expressioncomparator.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

#include "BioModels/FeatureCollection.h"
#include "Utils/Sorter.h"
#include "Statistics/Correlator.h"

namespace ExpressionComparator {

/**
 * @brief findClusterTissueCorrelations
 * @param clusters
 * @param tissues
 * @return Sorted correlations between every cluster and every tissue
 */
QVector<QVector<QPair<QString, double>>> findClusterTissueCorrelations(QVector<FeatureCollection> clusters, QVector<FeatureCollection> tissues) {
    QVector<QVector<QPair<QString, double>>> tissueCorrelationsForAllClusters;
    tissueCorrelationsForAllClusters.reserve(clusters.length());

    // For every cluster
    for (int i = 0; i < clusters.length(); i++) {
        QVector<QPair<QString, double>> tissueClusterCorrelations;
        tissueClusterCorrelations.reserve(tissues.length());

        // go through every tissue type
        for (int j = 0; j < tissues.length(); j++) {

            // Create a list that will hold all expression counts of both the current tissue j and the current cluster i
            QVector<double> tissueFeatureExpressionCounts,
                            clusterFeatureExpressionCounts;

            // And for every feature in the tissue j
            for (int k = 0; k < clusters[i].getNumberOfFeatures(); k++) {

                // Grab the expression count of the current looked at feature k in the current tissue j
                double tissueFeatureExpressionCount = clusters[i].getFeatureExpressionCount(k);

                // And add it to the list
                clusterFeatureExpressionCounts.append(tissueFeatureExpressionCount);

                bool isFeatureAlsoExpressedInTissue = false;

                // go through every feature in the current cluster i and search for the current looked at tissue feature
                for (int l = 0; l < tissues[j].getNumberOfFeatures(); l++) {

                    // for this check whether the feature IDs are equal
                    bool isFeatureIDEqual = tissues[j].getFeatureID(l).compare(clusters[i].getFeatureID(k)) == 0;

                    // If so, this means the feature has been found in the current cluster and the feature is expressed in the cluster
                    if (isFeatureIDEqual) {
                        isFeatureAlsoExpressedInTissue = true;

                        // So add its expression count to the list
                        double tissueFeatureExpressionCount = tissues[j].getFeatureExpressionCount(l);

                        tissueFeatureExpressionCounts.append(tissueFeatureExpressionCount);

                        // And end the search for it in the current cluster
                        break;
                    }
                }

                // If the feature has not been found in the current cluster, it means it is not expressed
                // In this case, append 0 as the feature expression count to the list
                if (!isFeatureAlsoExpressedInTissue) {
                    tissueFeatureExpressionCounts.append(.0);
                }
            }

            // If all features with their correct expression counts have been found and aligned in the two lists, calculate the correlation
            double tissueClusterCorrelation = Correlator::calculateSpearmanCorrelation(tissueFeatureExpressionCounts, clusterFeatureExpressionCounts);

            // And add it to the correlations list
            tissueClusterCorrelations.append(qMakePair(tissues[j].ID, tissueClusterCorrelation));
        }

        // In the end, sort the found tissue correlations after the correlation coefficients
        std::sort(tissueClusterCorrelations.begin(), tissueClusterCorrelations.end(),
                  [](QPair<QString, double> pairA, QPair<QString, double> pairB) { return pairA.second > pairB.second; });

        // and add the sorted list to the list of clusters with correlations to all tissues
        tissueCorrelationsForAllClusters.append(tissueClusterCorrelations);
    }

    return tissueCorrelationsForAllClusters;
}

}
