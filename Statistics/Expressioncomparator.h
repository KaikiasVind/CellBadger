#ifndef EXPRESSIONCOMPARATOR_H
#define EXPRESSIONCOMPARATOR_H

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

#include "BioModels/Cluster.h"
#include "BioModels/Celltype.h"

class ExpressionComparator
{
public:
    ExpressionComparator();

//    QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelations(QVector<QPair<QPair<QString, QString>, QStringList>> cellTypeMarkers, QVector<QStringList> clusterFeatureExpressions);
    QVector<QVector<QPair<CellType, double>>> findCellTypeCorrelations(QVector<CellType> cellTypes, QVector<Cluster> clusters);
//    QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelationsCellWise(QHash <QString, QVector<QPair<QString, QString>>>, QVector<QStringList> clusterFeatureExpressions);


};

#endif // EXPRESSIONCOMPARATOR_H
