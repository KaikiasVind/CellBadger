#ifndef CSVREADER_H
#define CSVREADER_H

#include <QVector>
#include <QStringList>
#include <QString>
#include <QPair>
#include <QHash>

#include "BioModels/FeatureCollection.h"
#include "BioModels/Celltype.h"

namespace CSVReader
{
    extern QVector<FeatureCollection> read10xGenomicsClustersFromFile(QString csvFilePath, double meanCountCutOff, double foldChangeCutOff);
    extern QVector<FeatureCollection> readCellTypesFromPanglaoDBFile(QString csvFilePath);

    // DEPRECATED
    extern QVector<CellType> getCellTypesWithMarkers(QString csvFilePath);
}

#endif // CSVREADER_H
