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
extern QStringList getFileNames(const QStringList filePaths);

// ####################################### GUI #######################################

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions);

// This function is implemented in the Helper_impl.h file
template<typename F>
void openExportWidgetWithPlot(F plottingFunction, const QString title, const std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> expressionDataForSelectedGenes);

// #################################### FILTERING #####################################

extern QVector<FeatureCollection> findTopNMostExpressedGenes(const QVector<FeatureCollection> experiment, const int numberOfGenesToPop);
extern QVector<FeatureCollection> filterExpressedGenesAccordingToFilters(const QVector<FeatureCollection> experiment, const QStringList completeGeneIDs, const AnalysisConfigModel analysisConfigModel);

}
// In clude Helper_impl.h after namespace to avoid error due to reimplementation of the openExportWidgetWithPlot function
#include "Helper_impl.h"

#endif // HELPER_H

