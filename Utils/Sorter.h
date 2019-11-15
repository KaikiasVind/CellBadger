#ifndef SORTER_H
#define SORTER_H

#include <QVector>

#include "BioModels/Celltype.h"

/**
 * @brief The Sorter struct is a utility class for everything concerned with sorting
 */
class Sorter
{
private:
    // Private -> No default constructor for the outside. Only utility class
    Sorter();

public:
//    static void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods);
    static void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods);
};

#endif // SORTER_H
