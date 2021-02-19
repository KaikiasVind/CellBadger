#ifndef RINTEROPERATOR_H
#define RINTEROPERATOR_H

#include <QVector>
#include <QString>
#include <RInside.h>
#include <Rcpp.h>

class RInterOperator
{
public:
    RInterOperator();

        QVector<QPair<QString, QVector<double>>> calculateDifferentialExpressedGenes(const QString matrixFilePath, const QString clusteringInformationFilePath, const QVector<int> clustersToCompare);

private:
    RInside rSession;

    void loadNecessaryLibraries();
};

#endif // RINTEROPERATOR_H
