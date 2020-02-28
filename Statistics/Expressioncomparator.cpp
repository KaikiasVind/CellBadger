#include "Expressioncomparator.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

#include <iostream>
using std::cout;
using std::endl;

#include "BioModels/FeatureCollection.h"
#include "Utils/Sorter.h"
#include "Statistics/Correlator.h"

namespace ExpressionComparator {

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
