#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QStringList>
#include <QWidget>

namespace Helper
{

extern QString chopFileName(QString filePath);
extern QStringList openFileDialog(QWidget * parent, QStringList validMimeTypeExtensions, bool isAcceptsMultipleFiles);
extern QString saveFileDialog(QWidget * parent, QString validMimeTypeExtensions);

};

#endif // HELPER_H
