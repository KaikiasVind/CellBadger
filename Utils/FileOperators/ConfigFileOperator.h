#ifndef CONFIGFILEOPERATOR_H
#define CONFIGFILEOPERATOR_H

#include <QString>

#include "System/ConfigFile.h"

namespace ConfigFileOperator
{
    extern ConfigFile readConfigFile(QString configFilePath);
    extern void writeConfigFile(QString configFilePath);
    extern void createConfigFile(QString configFilePath);
    extern ConfigFile initializeConfigFile();

    extern bool isConfigFileExists(QString configFilePath);
    extern bool isFileExists(QString filePaths);
};

#endif // CONFIGFILEOPERATOR_H
