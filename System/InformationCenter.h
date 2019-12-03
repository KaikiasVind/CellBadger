#ifndef INFORMATIONCENTER_H
#define INFORMATIONCENTER_H

#include "System/ConfigFile.h"
#include "BioModels/FeatureCollection.h"

struct InformationCenter
{

    ConfigFile configFile;

    QVector<FeatureCollection> cellMarkersForTypes;
    QVector<FeatureCollection> xClusters;

    InformationCenter(ConfigFile configFile);

};

#endif // INFORMATIONCENTER_H
