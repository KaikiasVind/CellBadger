#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QFile>

#include "BioModels/FeatureCollection.h"

namespace Helper
{


// ####################################### MISC #######################################

extern QString chopFileName(QString filePath);

// ####################################### MISC #######################################


// ####################################### GUI #######################################

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString validMimeTypeExtensions);

// ####################################### GUI #######################################


// #################################### BIOMODELS ####################################

// FIXME: This looks horrible
extern std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, double, double> getFeatureCollectionsAsGenes(const QVector<FeatureCollection> featureCollections, const QStringList completeGeneIDs);
extern QVector<FeatureCollection> getGenesAsFeatureCollections(const std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> genesWithGeneExpressionsForClusters);

// #################################### BIOMODELS ####################################

}

#endif // HELPER_H
