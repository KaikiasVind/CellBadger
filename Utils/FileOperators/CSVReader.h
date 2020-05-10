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
    extern QVector<FeatureCollection> getClusterFeatureExpressions(QString csvFilePath, double cutOff);

    extern QVector<FeatureCollection> getTissuesWithGeneExpression(QString csvFilePath, double cutOff);
};

#endif // CSVREADER_H
