#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QFile>

namespace Helper
{

extern QString chopFileName(QString filePath);

extern QStringList openFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);

}

#endif // HELPER_H
