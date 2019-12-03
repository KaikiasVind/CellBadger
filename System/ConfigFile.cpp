#include "ConfigFile.h"

#include <QString>


ConfigFile::ConfigFile() {};

ConfigFile::ConfigFile(/*QString projectFilePath,*/ QString cellMarkersFilePath, QString clusterExpressionFilePath)
    : /*projectFilePath (projectFilePath),*/ cellMarkersFilePath (cellMarkersFilePath), clusterExpressionFilePath (clusterExpressionFilePath)
{}
