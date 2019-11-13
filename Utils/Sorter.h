#ifndef SORTER_H
#define SORTER_H

#include <QVector>

class Sorter
{
public:
    Sorter();

    void findHighestLikelyCellTypeMapping(QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods);
};

#endif // SORTER_H
