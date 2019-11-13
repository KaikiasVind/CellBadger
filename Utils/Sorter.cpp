#include "Sorter.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>

Sorter::Sorter(){}

void Sorter::findHighestLikelyCellTypeMapping(QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods) {
    qDebug() << "Start sorting.";
    QPair<QPair<QString, QString>, double> highestLikelyCellTypeMapping;
    for (QVector<QPair<QPair<QString, QString>, double>> cluster : clustersWithCellTypeMappingLikelihoods) {
        highestLikelyCellTypeMapping = qMakePair(qMakePair(QString("nAn"), QString("nAn")), 0);

        for (QPair<QPair<QString, QString>, double> cellTypeWithMappingLikelihood : cluster) {
            bool isHigherLikelihood = cellTypeWithMappingLikelihood.second > highestLikelyCellTypeMapping.second;

            if (isHigherLikelihood) {
                highestLikelyCellTypeMapping = cellTypeWithMappingLikelihood;
            }
        }
        qDebug() << "Cluster:" << clustersWithCellTypeMappingLikelihoods.indexOf(cluster) << "is:" << highestLikelyCellTypeMapping.first << "with likelihood of:" << highestLikelyCellTypeMapping.second;
    }
}
