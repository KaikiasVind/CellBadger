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

    static QVector<Cluster> getClusterFeatureExpressions(QString csvFilePath, double cutOff);
//  static   QVector<Cluster> getClusterFeatureExpressions(QString csvFilePath);

    static QVector<CellType> getCellTypesWithMarkers(QString csvFilePath);

    static QHash <QString, QVector<QPair<QString, QString>>> sortCsvByMarker(QString csvFilePath);
};

#endif // CSVREADER_H
