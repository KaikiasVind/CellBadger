#ifndef EXPRESSIONCOMPARATOR_H
#define EXPRESSIONCOMPARATOR_H

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

#include "BioModels/FeatureCollection.h"

namespace ExpressionComparator
{
    extern QVector<QVector<QPair<QString, double>>> findClusterCellFoldChangeCorrelations(QVector<FeatureCollection> clusters, QVector<FeatureCollection> cellTypes);
    extern QVector<QVector<QPair<QString, QPair<double, double>>>> calculateCellTypeFoldChangeSumsForClusters(QVector<FeatureCollection> clusters, QVector<FeatureCollection> cellTypes);
}

#endif // EXPRESSIONCOMPARATOR_H
