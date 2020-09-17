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

enum class AnalysisFilterMode {
    TOP_N,
    MANUAL,
    AUTOMATIC
};

extern QString chopFileName(QString filePath);

// ####################################### GUI #######################################

enum class ShownData {
    RPM,
    RAW_COUNTS,
    FOLD_CHANGES
};

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions);

// #################################### FILTERING #####################################


extern QVector<FeatureCollection> findTopNMostExpressedGenes(const QVector<FeatureCollection> experiment, int numberOfGenesToPop);

}

#endif // HELPER_H
