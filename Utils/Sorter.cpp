#include "Sorter.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>

#include "BioModels/Celltype.h"

/**
 * @brief Sorter::findHighestLikelyCellTypeMapping
 * @param clustersWithCellTypeMappingLikelihoods
 */
//void Sorter::findHighestLikelyCellTypeMapping(QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods) {
void Sorter::findHighestLikelyCellTypeMapping(QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods) {
    qDebug() << "Start sorting.";
//    QPair<QPair<QString, QString>, double> highestLikelyCellTypeMapping;
//    QPair<CellType, double> highestLikelyCellTypeMapping;
    double highestLikelyCellTypeMapping;
    QString cellType, tissueType;
//    for (QVector<QPair<QPair<QString, QString>, double>> cluster : clustersWithCellTypeMappingLikelihoods) {
    int i = 0;
    for (QVector<QPair<CellType, double>> cluster : clustersWithCellTypeMappingLikelihoods) {
//        highestLikelyCellTypeMapping = qMakePair(qMakePair(QString("nAn"), QString("nAn")), 0);
        QStringList bla;
        CellType celltype("nAn", "nAn", bla);
//        highestLikelyCellTypeMapping = qMakePair(celltype, 0);
        highestLikelyCellTypeMapping = 0;

//        for (QPair<QPair<QString, QString>, double> cellTypeWithMappingLikelihood : cluster) {
        for (QPair<CellType, double> cellTypeWithMappingLikelihood : cluster) {
//            bool isHigherLikelihood = cellTypeWithMappingLikelihood.second > highestLikelyCellTypeMapping.second;
            bool isHigherLikelihood = cellTypeWithMappingLikelihood.second > highestLikelyCellTypeMapping;

            // NEXT TO GO!
            if (isHigherLikelihood) {
//                highestLikelyCellTypeMapping = cellTypeWithMappingLikelihood;
                highestLikelyCellTypeMapping = cellTypeWithMappingLikelihood.second;
                cellType = cellTypeWithMappingLikelihood.first.ID;
                tissueType = cellTypeWithMappingLikelihood.first.associatedTissueType;
            }
        }
//        qDebug() << "Cluster:" << clustersWithCellTypeMappingLikelihoods.indexOf(cluster) << "is:" << highestLikelyCellTypeMapping.first << "with likelihood of:" << highestLikelyCellTypeMapping.second;
        qDebug() << "Cluster:" << i << "is" << cellType << "/" << tissueType << "with 'likelihood' of" << highestLikelyCellTypeMapping;
        i++;
    }
}
