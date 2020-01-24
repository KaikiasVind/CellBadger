#ifndef INFORMATIONCENTER_H
#define INFORMATIONCENTER_H

#include <QVector>
#include <QPair>
#include <QString>

#include "System/ConfigFile.h"
#include "BioModels/FeatureCollection.h"

struct InformationCenter
{

    ConfigFile configFile;

    QVector<FeatureCollection> cellMarkersForTypes;
    QVector<QVector<FeatureCollection>> xClusterCollections;

    // FIXME: This looks very ugly!
    QVector<QVector<QVector<QPair<QString, double>>>> clustersWithTissueCorrelations;

    InformationCenter(ConfigFile configFile);
};

#endif // INFORMATIONCENTER_H
