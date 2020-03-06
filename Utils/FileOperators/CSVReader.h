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
    extern QVector<FeatureCollection> getClusterFeatures(QString csvFilePath, double meanCountCutOff, double foldChangeCutOff);

    extern QVector<CellType> getCellTypesWithMarkers(QString csvFilePath);

    extern QHash <QString, QVector<QPair<QString, QString>>> sortCsvByMarker(QString csvFilePath);

    extern QVector<FeatureCollection> getTissuesWithGeneExpression(QString csvFilePath, double cutOff);

    extern QVector<FeatureCollection> getUIAndSensitivityAndSpecicifityForMarkers(QString csvFilePath);
}

#endif // CSVREADER_H
