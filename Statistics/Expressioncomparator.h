#ifndef EXPRESSIONCOMPARATOR_H
#define EXPRESSIONCOMPARATOR_H

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

#include "BioModels/FeatureCollection.h"
#include "BioModels/Celltype.h"

namespace ExpressionComparator
{
    extern QVector<QVector<QPair<CellType, double>>> findCellTypeCorrelations(QVector<CellType> cellTypes, QVector<FeatureCollection> clusters);
//    extern QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelationsCellWise(QHash <QString, QVector<QPair<QString, QString>>>, QVector<QStringList> clusterFeatureExpressions);

    extern QVector<QVector<QPair<QString, double>>> findClusterTissueCorrelations(QVector<FeatureCollection> clusters, QVector<FeatureCollection> tissues);
};

#endif // EXPRESSIONCOMPARATOR_H
