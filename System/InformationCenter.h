#ifndef INFORMATIONCENTER_H
#define INFORMATIONCENTER_H

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

#include "System/ConfigFile.h"
#include "BioModels/FeatureCollection.h"

struct InformationCenter
{

    ConfigFile configFile;

    QStringList datasetFilePaths;

    QStringList completeSetOfGeneIDs;
    QVector<FeatureCollection> cellMarkersForTypes;
    QVector<QVector<FeatureCollection>> xClusterCollections;

    // FIXME: This looks very ugly!
    QVector<QVector<QVector<QPair<QString, double>>>> correlatedDatasets;

    InformationCenter(ConfigFile configFile);
};

#endif // INFORMATIONCENTER_H
