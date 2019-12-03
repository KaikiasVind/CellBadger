#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QString>

struct ConfigFile
{
//    QString projectFilePath;
    QString cellMarkersFilePath;
    QString clusterExpressionFilePath;

    ConfigFile();
    ConfigFile(/*QString projectFilePath,*/ QString cellMarkersFilePath, QString clusterExpressionFilePath);
};

#endif // CONFIGFILE_H
