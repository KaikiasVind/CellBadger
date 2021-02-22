#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QFile>
#include <QtCharts/QChartView>
#include <QFileDialog>
#include <QTableView>

#include "BioModels/FeatureCollection.h"
#include "Utils/Models/AnalysisConfigModel.h"

namespace Helper
{

// ####################################### MISC #######################################

extern QString chopFileName(QString filePath);
extern QStringList getFileNames(const QStringList filePaths);
extern void exitIfErrorOnOpen(QFile * file, const QString filePath);
extern void saveTableEntriesToCSV(QWidget * parent, QTableView * table);

// ####################################### GUI #######################################

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, QFileDialog::FileMode fileMode);
extern QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions);
extern void openExportWidgetWithPlot(QtCharts::QChartView * chart);

// #################################### FILTERING #####################################

extern QVector<FeatureCollection> findTopNMostExpressedGenes(const QVector<FeatureCollection> experiment, const int numberOfGenesToPop);
extern QVector<FeatureCollection> filterExpressedGenesAccordingToFilters(const QVector<FeatureCollection> experiment, const QStringList completeGeneIDs, const AnalysisConfigModel analysisConfigModel);

}

#endif // HELPER_H

