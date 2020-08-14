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
extern double calculateMeanRawCountForGene(QString geneID, QVector<FeatureCollection> clusters);

// ####################################### GUI #######################################

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions);

}

#endif // HELPER_H
