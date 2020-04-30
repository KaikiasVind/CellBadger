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

#define normalized 1
namespace ExpressionComparator {

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

/**
 * @brief findMostLikelyCellTypes - ...
 * @param clusters - List of 10x Clusters
 * @param cellTypes - List of panglaodb cell types
 * @param sensitivityRange - (x,y) with x = lowest accepted sensitivity, y = lowest value still considered high sensitivity
 * @param specificityRange - (x,y) with x = highest value still considered low specificity, y = highest accepted specificity
 * @param minimumSpan - Minimum accepted distance between the sensitivity and specificity value
 * @return
 */
QVector<QVector<QPair<QString, double>>> findMostLikelyCellTypes(QVector<FeatureCollection> clusters, QVector<FeatureCollection> cellTypes, QPair<double, double> sensitivityRange, QPair<double, double> specificityRange, double minimumSpan) {
    QVector<QVector<QPair<QString, double>>> mostLikelyCellTypes;

    // Lay out information about sensitivity and specificity
    // LowestHighSensitivity means that everything bigger than this value is regared as high sensitivity
    double lowestAcceptedSensitivity = sensitivityRange.first,
           lowestHighSensitivity = sensitivityRange.second;

    // HighestLowSpecificity means that everything lower than this value is regarded as low specificity
    double highestLowSpecificity = specificityRange.first,
           highestAcceptedSpecificity = specificityRange.second;

    QVector<QVector<Feature>> relevantGenesForCellTypes;

    // Go through every cluster that was parsed from the 10x cluster file
    for (int i = 0; i < clusters.length(); i++) {

        // This is used to store the cell type ID that is currently considered most likely
        QString highestLikelyCellType;

        // And go through every cell type that was taken from the pangloDB marker file
        for (int j = 0; j < cellTypes.length(); j++) {

            // Find genes that are expresed in the cluster and are found in the cell type
            FeatureCollection cluster = clusters[i],
                              cellType = cellTypes[j];

            // For every feature in the current cluster
            for (int k = 0; k < cluster.getNumberOfFeatures(); k++) {

                // This is used to store the amount of genes in the clusters that are considered relevant
                // And compare it with the cell type that - so far - has been considered most likely
                // see far below for more
                int highestNumberOfRelevantGenes = 0,
                    numberOfRelevantGenes = 0;

                // Grab all genes that the current cluster and the current cell type have in common
                for (int l = 0; l < cellType.getNumberOfFeatures(); l++) {

                    // If a common gene is found, check if it lies within the given range for sensitivity and specificity
                    // Otherwise it is dropped
                    if (cellType.getFeatureID(l).compare(cluster.getFeatureID(k)) == 0) {
                        Feature foundCommonGene = cellType.getFeature(l);

                        // Collect the gene's sensitivity and specificity
                        double sensitivity = foundCommonGene.sensitivity,
                               specificity = foundCommonGene.specificity,
                               sensitivitySpecificitySpan = abs((sensitivity > specificity) ? (sensitivity - specificity) : (specificity - sensitivity));

                        // Check whether the gene's sensitivity for the current cell type is too high or too low
                        // and whether the gene's values for sensitivity and specificity don't lie too close together
                        bool isSensitivityTooLow = sensitivity < lowestAcceptedSensitivity,
                             isSpecificityTooHigh = specificity > highestAcceptedSpecificity,
                             isEnoughDistance = sensitivitySpecificitySpan > minimumSpan;

                        // If that's the case, drop it
                        if (isSensitivityTooLow || isSpecificityTooHigh) {
                            continue;
                        }

                        // Check whether the gene's sensitivity for the current cell type would be considered high or low
                        // regarding to the given ranges
                        bool isHighSensitivity = sensitivity > lowestHighSensitivity,
                             isLowSpecificity  = specificity < highestLowSpecificity;

                        // If the gene's sensitivity and specificity are both considered high which means it is not
                        // explicitly highly expressed in this celltype only, this could be a housekeeping gene
                        // In this case, it won't bring valuable information, so drop it.
                        if (isHighSensitivity && !isLowSpecificity) {
                            continue;
                        }

                        // If the gene's sensitivity and specificity are both considered low which means it is neither
                        // expressed explicitly anywhere, neither in this particular cell type nor in another, this won't
                        // bring valuable information, so drop it. Otherwise it will add to potential background noise.
                        else if (!isHighSensitivity && isLowSpecificity) {
                            continue;
                        }

                        // If the gene's value for sensitivity and specificity lie too close together
                        // the gene will not bring valuable information and only add up to potential background noise.
                        // If that's the case, drop it.
                        else if (!isEnoughDistance) {
                            continue;
                        }

                        // If all of the above cases didn't lead to this gene being dropped,
                        // this gene has a high sensitivity for the current cell type and a low
                        // specificity for others, which means it is very much exclusively expressed in this
                        // cell type which classifies it as a relevant cell marker gene
                        else {
                            numberOfRelevantGenes++;
                        }

                        //REMEMBER: Was ist, wenn die beide nicht hoch sind, aber weit genug auseinander liegen?
                    }
                }

                // If the number of relevant genes - that has so far been noted for the highest likely type
                // is lower than the number of relevant genes that are noted for the current cell type
                // mark the current cell type as new highest likely
                if (numberOfRelevantGenes > highestNumberOfRelevantGenes) {
                    highestNumberOfRelevantGenes = numberOfRelevantGenes;
                    highestLikelyCellType = cellTypes.at(j).ID;
                }
            }
        }
        qDebug() << "Most likely cell type:" << highestLikelyCellType;
    }

    return mostLikelyCellTypes;
}



// REMEMBER: Refactor!!!!
/**
 * @brief findClusterTissueCorrelations - Correlates EVERY given cluster to EVERY given tissue - Runs in O(n*m) at least
 * @param clusters - List of clusters (CellRanger t-SNE clusters)
 * @param tissues - List of tissues (One tissue contains cell markers with corresponding expression counts - see BioModels/FeatureCollection)
 * @return - Sorted correlations of every cluster and every tissue
 */
QVector<QVector<QPair<QString, double>>> findClusterTissueCorrelations(QVector<FeatureCollection> clusters, QVector<FeatureCollection> tissues) {

    // Create a list that will contain the correlation of all clusters with all tissues
    QVector<QVector<QPair<QString, double>>> tissueCorrelationsForAllClusters;
    tissueCorrelationsForAllClusters.reserve(clusters.length());

    // For every cluster from the list of clusters that was given
    for (int i = 0; i < clusters.length(); i++) {

        // Create a list that will contain the correlation of the current cluster i with all tissues
        QVector<QPair<QString, double>> clusterTissueCorrelations;
        clusterTissueCorrelations.reserve(tissues.length());

        // Go through every tissue type and correlate the cluster with this tissue type
        for (int j = 0; j < tissues.length(); j++) {

            // Find the features that are expressed in both the current cluster i and the current tissue j
            QVector<QPair<Feature, Feature>> equallyExpressedFeatures = Sorter::findEquallyExpressedFeatures(clusters[i], tissues[j]);

            int numberOfEquallyExpressedFeatures = equallyExpressedFeatures.length();

            // Create lists that will only hold the feature counts for both the current cluster i and the current tissue j
            // This is done to calculate the correlation the feature counts only
            QVector<double> tissueFeatureExpressionCounts,
                            clusterFeatureExpressionCounts;
            tissueFeatureExpressionCounts.reserve(numberOfEquallyExpressedFeatures);
            clusterFeatureExpressionCounts.reserve(numberOfEquallyExpressedFeatures);

            // Fill the two created lists with the feature counts.
            for (QPair<Feature, Feature> equallyExpressedFeature : equallyExpressedFeatures) {
                clusterFeatureExpressionCounts.append(equallyExpressedFeature.first.count);
                tissueFeatureExpressionCounts.append(equallyExpressedFeature.second.count);
            }

            // Correlate the two lists of feature counts.
            double correlation = Correlator::calculateSpearmanCorrelation(clusterFeatureExpressionCounts, tissueFeatureExpressionCounts);

            // Now append the calculated correlation to the list of correlations
            // The correlation that is added here is the correllation between the current cluster i and the current tissue type j
            clusterTissueCorrelations.append(qMakePair(tissues[j].ID, correlation));
        }

        // Now sort the found correlations due to place the tissue with the highest correlation to the cluster on top of the list
        // Be aware that the sorting is IN PLACE -> This directly alters the list clusterTissueCorrelations
        //REMEMBER: This has no place here, put it outside of this function
        std::sort(clusterTissueCorrelations.begin(), clusterTissueCorrelations.end(),
                  [](QPair<QString, double> pairA, QPair<QString, double> pairB) { return pairA.second > pairB.second; });

        // Now add the finished correlations of cluster i with every tissue to the final list
        tissueCorrelationsForAllClusters.append(clusterTissueCorrelations);
    }

    // Return the final list including the correlations of all clusters with all tissues
    return tissueCorrelationsForAllClusters;
}


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

            for (int k = 0; k < clusters[i].getNumberOfFeatures(); k++) {

                double clusterFeatureFoldChange = clusters[i].getFeatureFoldChange(k);

                clusterFeatureFoldChanges.append(clusterFeatureFoldChange);

                bool isFeatureExpressedInCellType = false;

                for (int l = 0; l < cellTypes[j].getNumberOfFeatures(); l++) {

                    bool isFeatureIDEqual = clusters[i].getFeatureID(k).compare(cellTypes[j].getFeatureID(l)) == 0;

                    if (isFeatureIDEqual) {
                        isFeatureExpressedInCellType = true;

                        double cellTypeFeatureFoldChange = cellTypes[j].getFeatureFoldChange(l);

                        cellTypeFeatureFoldChanges.append(cellTypeFeatureFoldChange);

                        // End end the search for it in the current cluster
                        break;
                    }
                }

                // If the feature has not been found in the cell type, this means it is not expressed in the cell type
                // In this case append the neutral value 1 to the fold changes list
                if (!isFeatureExpressedInCellType) {
                    cellTypeFeatureFoldChanges.append(0.0);
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


QVector<QVector<QPair<CellType, double>>> findCellTypeCorrelations(QVector<CellType> cellTypes, QVector<FeatureCollection> clusters) {
    QVector<QVector<QPair<CellType, double>>> clustersWithCellMappingLikelihoods;

    qDebug() << "Go: Find cell type correlations";
    for (FeatureCollection cluster : clusters) {
        QVector<QPair<CellType, double>> cellMappingLikelihoods;

        for (CellType cellType : cellTypes) {
            int numberOfFeatures = cellType.associatedMarkers.length();
            int numberOfExpressedFeatures = 0;

            for (QString marker : cellType.associatedMarkers) {
                bool isMarkerExpressed = cluster.isFeatureExpressed(marker);

                if (isMarkerExpressed) {
                    numberOfExpressedFeatures++;
                }
            }

            double mappingLikelihood = double(numberOfExpressedFeatures) / double(numberOfFeatures);
            cellMappingLikelihoods.append(qMakePair(cellType, mappingLikelihood));
        }
        clustersWithCellMappingLikelihoods.append(cellMappingLikelihoods);
    }

    qDebug() << "Finished";
    return clustersWithCellMappingLikelihoods;

//    for (QPair<QString, QStringList> cellTypeWithMarkers : cellTypesWithAssociatedMarkers) {

//        QVector<QPair<QString, double>> mappingLikelihoods;
//        int numberOfFeatures = cellTypeWithMarkers.second.length();
//        int numberOfExpressedFeaturesInCluster = 0;

//        qDebug() << "Checking correlation with:" << cellTypeWithMarkers.first;
//        for (int i = 0; i < cellTypeWithMarkers.second.length(); i++) {
//            QString currentMarker = cellTypeWithMarkers.second[i];
//            bool isMarkerExpressed = clusterFeatureExpressions[i].contains(currentMarker);

//            if (isMarkerExpressed) {
//                numberOfExpressedFeaturesInCluster++;
//            }
//        }

//        double mappingLikelihood = double(numberOfExpressedFeaturesInCluster) / double(numberOfFeatures);

//        QString currentCellType = cellTypeWithMarkers.first;
////        qDebug() << "Likelihood for" << currentCellType << ":" << mappingLikelihood;
//        mappingLikelihoods.append(qMakePair(currentCellType, mappingLikelihood));
//    }
}

//QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelationsCellWise(QHash <QString, QVector<QPair<QString, QString>>> cellMarkersWithAssociatedTissues, QVector<QStringList> clusterFeatureExpressions) {

//    qDebug() << "Go: Find cell type correlations cellwise";
//    for (QStringList cluster : clusterFeatureExpressions) {
//        QVector<QVector<QPair<QString, QString>>> mappedTissuesPerCell;

//        for (QString expressedCellFeature : cluster) {
//            QVector<QPair<QString, QString>> associatedTissueTypes = cellMarkersWithAssociatedTissues.value(expressedCellFeature);

//        }
//    }

////    QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellMappingLikelihoods;

////    qDebug() << "Go: Find cell type correlations cellwise";
////    for (QStringList cluster : clusterFeatureExpressions) {
////        QVector<QPair<QString, QString>> cellTypeMappingsPerCell;

////        for (QString cell : cluster) {
////            QPair<QString, QString> cellTypeMappingForCell;

////            for (QPair<QPair<QString, QString>, QStringList> cellType : cellTypesWithAssociatedMarkers) {

////                for (QString marker : cellType.second) {
////                    bool isMarkerExpressed = cell
////                }
////            }
////        }
////    }


////    for (QStringList cluster : clusterFeatureExpressions) {
////        QVector<QPair<QPair<QString, QString>, double>> cellMappingLikelihoods;

////        for (QPair<QPair<QString, QString>, QStringList> cellType : cellTypesWithAssociatedMarkers) {
////            int numberOfFeatures = cellType.second.length();
////            int numberOfExpressedFeatures = 0;

////            for (QString marker : cellType.second) {
////                bool isMarkerExpressed = cluster.contains(marker);

////                if (isMarkerExpressed) {
////                    numberOfExpressedFeatures++;
////                }
////            }

////            double mappingLikelihood = double(numberOfExpressedFeatures) / double(numberOfFeatures);
////            cellMappingLikelihoods.append(qMakePair(cellType.first, mappingLikelihood));
////        }
////        clustersWithCellMappingLikelihoods.append(cellMappingLikelihoods);
////    }

//    qDebug() << "Finished";
//    return clustersWithCellMappingLikelihoods;

//}

}
