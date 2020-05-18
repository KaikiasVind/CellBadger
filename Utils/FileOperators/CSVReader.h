#ifndef CSVREADER_H
#define CSVREADER_H

#include <QVector>
#include <QStringList>
#include <QString>
#include <QPair>
#include <QHash>

#include "BioModels/FeatureCollection.h"

namespace CSVReader
{
    extern QVector<FeatureCollection> read10xGenomicsClustersFromFile(const QString csvFilePath, const QVector<double> cutOffs);
    extern QVector<FeatureCollection> readCellTypesFromPanglaoDBFile(const QString csvFilePath, const QVector<double> cutOffs);

    // DEPRECATED
//    extern QVector<CellType> getCellTypesWithMarkers(QString csvFilePath);
}

#endif // CSVREADER_H
