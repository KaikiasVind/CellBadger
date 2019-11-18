#ifndef CSVREADER_H
#define CSVREADER_H

#include <QVector>
#include <QStringList>
#include <QString>
#include <QPair>
#include <QHash>

#include "BioModels/Cluster.h"
#include "BioModels/Celltype.h"

namespace CSVReader
{
    extern QVector<Cluster> getClusterFeatureExpressions(QString csvFilePath, double cutOff);
//  extern QVector<Cluster> getClusterFeatureExpressions(QString csvFilePath);

    extern QVector<CellType> getCellTypesWithMarkers(QString csvFilePath);

    extern QHash <QString, QVector<QPair<QString, QString>>> sortCsvByMarker(QString csvFilePath);
};

#endif // CSVREADER_H
