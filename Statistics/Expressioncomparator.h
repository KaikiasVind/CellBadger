#ifndef EXPRESSIONCOMPARATOR_H
#define EXPRESSIONCOMPARATOR_H

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

#include "BioModels/FeatureCollection.h"

namespace ExpressionComparator
{
    extern QVector<QPair<QString, double>> findClusterCellTypeFoldChangeCorrelations(FeatureCollection cluster, QVector<FeatureCollection> cellTypes);
    extern QVector<QVector<QPair<QString, double>>> findCellFoldChangeCorrelationsForAllClusters(QVector<FeatureCollection> clusters, QVector<FeatureCollection> cellTypes);
    extern QVector<QVector<QPair<QString, QPair<double, double>>>> calculateCellTypeFoldChangeSumsForClusters(QVector<FeatureCollection> clusters, QVector<FeatureCollection> cellTypes);

// +++++++++++++++++++++++++++++++++++++++++++++++++++++ DEPRECATED +++++++++++++++++++++++++++++++++++++++++++++++++++++
//    extern QVector<QVector<QPair<CellType, double>>> findCellTypeCorrelations(QVector<CellType> cellTypes, QVector<FeatureCollection> clusters);
//    extern QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelationsCellWise(QHash <QString, QVector<QPair<QString, QString>>>, QVector<QStringList> clusterFeatureExpressions)
//    extern QVector<QVector<QPair<QString, double>>> findClusterTissueCorrelations(QVector<FeatureCollection> clusters, QVector<FeatureCollection> tissues);

}

#endif // EXPRESSIONCOMPARATOR_H
