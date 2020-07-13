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

    QVector<QStringList> completeSetsOfGeneIDsPerDataset;
    QVector<FeatureCollection> cellMarkersForTypes;
    QVector<QVector<FeatureCollection>> xClusterCollections;

    // FIXME: This looks very ugly!
    QVector<QVector<QVector<QPair<QString, double>>>> correlatedDatasets;
    QVector<QVector<double>> qualityScores;

    InformationCenter(ConfigFile configFile);
};

#endif // INFORMATIONCENTER_H
