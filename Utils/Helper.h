#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QFile>
#include <QtCharts/QChartView>

#include "BioModels/FeatureCollection.h"

namespace Helper
{

// ####################################### MISC #######################################

extern QString chopFileName(QString filePath);
extern QStringList getFileNames(const QStringList filePaths);

// ####################################### GUI #######################################

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions);
extern void openExportWidgetWithPlot(QtCharts::QChartView * chart);

// #################################### FILTERING #####################################

extern QVector<FeatureCollection> findTopNMostExpressedGenes(const QVector<FeatureCollection> experiment, const int numberOfGenesToPop);

}

#endif // HELPER_H

