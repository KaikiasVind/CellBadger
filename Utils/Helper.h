#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QFile>

namespace Helper
{


// ####################################### MISC #######################################

extern QString chopFileName(QString filePath);

// ####################################### MISC #######################################


// ####################################### GUI #######################################

extern QStringList openLoadFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString openSaveFileDialog(QWidget * parent, QString validMimeTypeExtensions);

// ####################################### GUI #######################################

}

#endif // HELPER_H
