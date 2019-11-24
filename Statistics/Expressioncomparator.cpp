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
 * @return
 */
QVector<QVector<QPair<QString, double>>> findClusterTissueCorrelations(QVector<FeatureCollection> clusters, QVector<FeatureCollection> tissues) {
    QVector<QVector<QPair<QString, double>>> tissueCorrelationsForAllClusters;
    tissueCorrelationsForAllClusters.reserve(clusters.length());

    for (int i = 0; i < clusters.length(); i++) {
        QVector<QPair<QString, double>> clusterTissueCorrelations;
        clusterTissueCorrelations.reserve(tissues.length());

        for (int j = 0; j < tissues.length(); j++) {
            QVector<QPair<Feature, Feature>> equallyExpressedFeatures = Sorter::findEquallyExpressedFeatures(clusters[i], tissues[j]);

            int numberOfEquallyExpressedFeatures = equallyExpressedFeatures.length();

            QVector<double> tissueFeatureExpressionCounts,
                            clusterFeatureExpressionCounts;
            tissueFeatureExpressionCounts.reserve(numberOfEquallyExpressedFeatures);
            clusterFeatureExpressionCounts.reserve(numberOfEquallyExpressedFeatures);
            for (QPair<Feature, Feature> equallyExpressedFeature : equallyExpressedFeatures) {
                clusterFeatureExpressionCounts.append(equallyExpressedFeature.first.count);
                tissueFeatureExpressionCounts.append(equallyExpressedFeature.second.count);
            }

            double correlation = Correlator::calculateSpearmanCorrelation(clusterFeatureExpressionCounts, tissueFeatureExpressionCounts);

            clusterTissueCorrelations.append(qMakePair(tissues[j].ID, correlation));
        }

        std::sort(clusterTissueCorrelations.begin(), clusterTissueCorrelations.end(),
                  [](QPair<QString, double> pairA, QPair<QString, double> pairB) { return pairA.second > pairB.second; });

        tissueCorrelationsForAllClusters.append(clusterTissueCorrelations);
    }

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
