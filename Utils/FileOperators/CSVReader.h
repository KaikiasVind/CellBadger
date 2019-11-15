#ifndef CSVREADER_H
#define CSVREADER_H

#include <QVector>
#include <QStringList>
#include <QString>
#include <QPair>
#include <QHash>

#include "BioModels/Cluster.h"
#include "BioModels/Celltype.h"

class CSVReader
{
public:
    CSVReader();

//    QVector<Cluster> getClusterFeatureExpressions(QString csvFilePath, double cutOff);
    QVector<Cluster> getClusterFeatureExpressions(QString csvFilePath);

//    QVector<QPair<QPair<QString, QString>, QStringList>> getCellTypeMarkers(QString csvFilePath);
    QVector<CellType> getCellTypesWithMarkers(QString csvFilePath);

    QHash <QString, QVector<QPair<QString, QString>>> sortCsvByMarker(QString csvFilePath);
};

#endif // CSVREADER_H
