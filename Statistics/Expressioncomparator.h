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
    static QVector<QVector<QPair<CellType, double>>> findCellTypeCorrelations(QVector<CellType> cellTypes, QVector<Cluster> clusters);
//    QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelationsCellWise(QHash <QString, QVector<QPair<QString, QString>>>, QVector<QStringList> clusterFeatureExpressions);

private:
    ExpressionComparator();

};

#endif // EXPRESSIONCOMPARATOR_H
