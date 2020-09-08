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
enum class ShownData {
    RPM,
    RAW_COUNTS,
    FOLD_CHANGES
};

extern QString chopFileName(QString filePath);

// ####################################### GUI #######################################
extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString description, QString validMimeTypeExtensions);

}

#endif // HELPER_H
