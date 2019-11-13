#ifndef EXPRESSIONCOMPARATOR_H
#define EXPRESSIONCOMPARATOR_H

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

class ExpressionComparator
{
public:
    ExpressionComparator();

    QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelations(QVector<QPair<QPair<QString, QString>, QStringList>> cellTypeMarkers, QVector<QStringList> clusterFeatureExpressions);
//    QVector<QVector<QPair<QPair<QString, QString>, double>>> findCellTypeCorrelationsCellWise(QHash <QString, QVector<QPair<QString, QString>>>, QVector<QStringList> clusterFeatureExpressions);


};

#endif // EXPRESSIONCOMPARATOR_H
