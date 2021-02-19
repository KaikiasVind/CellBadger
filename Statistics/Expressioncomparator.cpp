#include "Expressioncomparator.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>
#include <math.h>

#include "BioModels/FeatureCollection.h"
#include "Utils/Sorter.h"
#include "Statistics/Correlator.h"

namespace ExpressionComparator {


/**
 * @brief findClusterTissueCorrelations - Correlates EVERY given cluster to EVERY given tissue - Runs in O(n*m) at least
 * @param clusters - List of clusters (CellRanger t-SNE clusters)
 * @param tissues - List of tissues (One tissue contains cell markers with corresponding expression counts - see BioModels/FeatureCollection)
 * @return - Sorted correlations of every cluster and every tissue
 */
QVector<QVector<QPair<QString, double>>> findClusterCellFoldChangeCorrelations(QVector<FeatureCollection> clusters, QVector<FeatureCollection> cellTypes) {

    // Create a list that will contain the correlation of all clusters with all cell types
    QVector<QVector<QPair<QString, double>>> cellTypeCorrelationsForAllClusters;
    cellTypeCorrelationsForAllClusters.reserve(clusters.length());

    // For every cluster from the list of clusters that was given
    for (int i = 0; i < clusters.length(); i++) {

        QVector<QPair<QString, double>> cellTypeClusterCorrelations;
        cellTypeClusterCorrelations.reserve(cellTypes.length());

        // Create a list that will contain the correlation of the current cluster i with all cell types
        QVector<QPair<QString, double>> cellTypeTissueCorrelations;
        cellTypeTissueCorrelations.reserve(cellTypes.length());

        // Go through every tissue type and correlate the cluster with this tissue type
        for (int j = 0; j < cellTypes.length(); j++) {

            QVector<double> clusterFeatureFoldChanges,
                            cellTypeFeatureFoldChanges;

            for (int k = 0; k < cellTypes[j].getNumberOfFeatures(); k++) {

                double cellTypeFeatureFoldChange = cellTypes[j].getFeatureFoldChange(k);

                cellTypeFeatureFoldChanges.append(cellTypeFeatureFoldChange);

                bool isFeatureExpressedInCluster = false;

                for (int l = 0; l < clusters[i].getNumberOfFeatures(); l++) {

                    bool isFeatureIDEqual = cellTypes[j].getFeatureID(k).compare(clusters[i].getFeatureID(l)) == 0;

                    if (isFeatureIDEqual) {
                        isFeatureExpressedInCluster = true;

                        double clusterFeatureFoldChange = clusters[i].getFeatureFoldChange(l);

                        clusterFeatureFoldChanges.append(clusterFeatureFoldChange);

                        // End end the search for it in the current cluster
                        break;
                    }
                }

                // If the feature has not been found in the cell type, this means it is not expressed in the cell type
                // In this case append the neutral value 1 to the fold changes list
                if (!isFeatureExpressedInCluster) {
                    clusterFeatureFoldChanges.append(1.0);
                }
            }

            // Correlate the two lists of feature counts.
            double correlation = Correlator::calculateSpearmanCorrelation(clusterFeatureFoldChanges, cellTypeFeatureFoldChanges);

            // Now append the calculated correlation to the list of correlations
            // The correlation that is added here is the correllation between the current cluster i and the current tissue type j
            cellTypeClusterCorrelations.append(qMakePair(cellTypes[j].ID, correlation));
        }

        // Now sort the found correlations due to place the tissue with the highest correlation to the cluster on top of the list
        // Be aware that the sorting is IN PLACE -> This directly alters the list clusterTissueCorrelations
        //REMEMBER: This has no place here, put it outside of this function
        std::sort(cellTypeClusterCorrelations.begin(), cellTypeClusterCorrelations.end(),
                  [](QPair<QString, double> pairA, QPair<QString, double> pairB) { return pairA.second > pairB.second; });

        // Now add the finished correlations of cluster i with every tissue to the final list
        cellTypeCorrelationsForAllClusters.append(cellTypeClusterCorrelations);
    }

    // Return the final list including the correlations of all clusters with all tissues
    return cellTypeCorrelationsForAllClusters;
}


/**
 * @brief calculateCellTypeFoldChangeSumsForClusters - For every cluster the the sum of all fold changes of every cell type is calculated
 * @param clusters - List of 10xGenomics clusters
 * @param cellTypes - List of GTEx cell types
 * @return
 */
QVector<QVector<QPair<QString, QPair<double, double>>>> calculateCellTypeFoldChangeSumsForClusters(QVector<FeatureCollection> clusters, QVector<FeatureCollection> cellTypes) {

    // Create a list that will hold a list per cluster that itself holdes the fold change sums of all cell types regarding the current cluster
    QVector<QVector<QPair<QString, QPair<double, double>>>> cellTypesWithFoldChangeSumsForClusters;

    // Go through every cluster in the given cluster file
    for (int i = 0; i < clusters.length(); i++) {

        QVector<QPair<QString, QPair<double, double>>> cellTypesWithFoldChangeSums;

        // Go through every cell type that is present in the PanglaoDB file
        for (int j = 0; j < cellTypes.length(); j++) {

            // Calculate the sum of all fold changes of all features in cluster i also expressed in the cell type j
            // Independently for the cluster i and for the cell type j
            double clusterGenesFoldChangeSum = 0,
                   cellTypeGenesFoldChangeSum = 0;

            int numberOfEquallyExpressedGenes = 0;

            // Go through every expressed feature in the cluster i and compute the fold change sums
            for (int k = 0; k < clusters[i].getNumberOfFeatures(); k++) {

                bool isGeneExpressed = false;

                // Add the gene fold change to the fold change sum
                clusterGenesFoldChangeSum += clusters[i].getFeature(k).foldChange;

                // Go through every expressed feature in the cell type j and add the fold changes of those genes
                // to the list, which can be found in the cluster
                for (int l = 0; l < cellTypes[j].getNumberOfFeatures(); l++) {

                    // Compare the gene IDs of the currently watched features
                    bool isGeneIDEqual = clusters[i].getFeatureID(k).compare(cellTypes[j].getFeatureID(l)) < 0;

                    // If a gene with the same gene ID has been found, add it's fold change to the fold change sums
                    // Independently for the cluster i and for the cell type j
                    if (isGeneIDEqual) {
                        isGeneExpressed = true;
                        cellTypeGenesFoldChangeSum += cellTypes[j].getFeature(l).foldChange;
                        numberOfEquallyExpressedGenes++;
                    }
                }

                // If the gene has not been found in the cell type, which means the gene is not expressed in the cell type
                // add the neutral value 1 to the fold change sum
                if (!isGeneExpressed) {
                    cellTypeGenesFoldChangeSum += 1.0;
                }
            }

            // Normalize the fold change sums with the number of expressed genes
//            double normalizedClusterGenesFoldChangeSum = (clusterGenesFoldChangeSum / clusters[i].getNumberOfFeatures()),
//                   normalizedCellTypeGenesFoldChangeSum = (cellTypeGenesFoldChangeSum / cellTypes[j].getNumberOfFeatures());

            // Calculate distance of the fold change sum of the current cell type j to the fold change sum of the current cluster i
            double distanceOfFoldChangeSums = qAbs(clusterGenesFoldChangeSum - cellTypeGenesFoldChangeSum);
//            double distanceOfFoldChangeSums = qAbs(normalizedClusterGenesFoldChangeSum - normalizedCellTypeGenesFoldChangeSum);

            // And add the cell type including it's fold change sum and distance to cluster fold change sum to the list
            cellTypesWithFoldChangeSums.append(qMakePair(cellTypes[j].ID, qMakePair(cellTypeGenesFoldChangeSum, distanceOfFoldChangeSums)));
//            cellTypesWithFoldChangeSums.append(qMakePair(cellTypes[j].ID, qMakePair(normalizedCellTypeGenesFoldChangeSum, distanceOfFoldChangeSums)));
        }

        // Add the list of cell types to the current cluster cell type list
        cellTypesWithFoldChangeSumsForClusters.append(cellTypesWithFoldChangeSums);
    }

    return cellTypesWithFoldChangeSumsForClusters;
}

}
