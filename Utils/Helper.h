#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QFile>

#include "BioModels/FeatureCollection.h"
#include "Utils/Models/AnalysisConfigModel.h"

namespace Helper
{

// ####################################### MISC #######################################

extern QString chopFileName(QString filePath);

// ####################################### GUI #######################################

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions);

// #################################### FILTERING #####################################

extern QVector<FeatureCollection> findTopNMostExpressedGenes(const QVector<FeatureCollection> experiment, const int numberOfGenesToPop);
extern QVector<FeatureCollection> filterExpressedGenesAccordingToFilters(const QVector<FeatureCollection> experiment, const QStringList completeGeneIDs, const AnalysisConfigModel analysisConfigModel);

}

#endif // HELPER_H
