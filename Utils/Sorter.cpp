#include "Sorter.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>

#include "BioModels/Celltype.h"

namespace Sorter {

/**
 * @brief Sorter::findHighestLikelyCellTypeMapping
 * @param clustersWithCellTypeMappingLikelihoods
 */
//void Sorter::findHighestLikelyCellTypeMapping(QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods) {
void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods) {
    qDebug() << "Start sorting.";
    double highestLikelyCellTypeMapping;
    QString cellType, tissueType;
    int i = 0;

    for (QVector<QPair<CellType, double>> cluster : clustersWithCellTypeMappingLikelihoods) {
        QStringList bla;
        CellType celltype("nAn", "nAn", bla);
        highestLikelyCellTypeMapping = 0;

        for (QPair<CellType, double> cellTypeWithMappingLikelihood : cluster) {
            bool isHigherLikelihood = cellTypeWithMappingLikelihood.second > highestLikelyCellTypeMapping;

            if (isHigherLikelihood) {
                highestLikelyCellTypeMapping = cellTypeWithMappingLikelihood.second;
                cellType = cellTypeWithMappingLikelihood.first.ID;
                tissueType = cellTypeWithMappingLikelihood.first.associatedTissueType;
            }
        }

        qDebug() << "Cluster:" << i << "is" << cellType << "/" << tissueType << "with 'likelihood' of" << highestLikelyCellTypeMapping;
        i++;
    }
}

}
