#ifndef CONFIGFILEOPERATOR_H
#define CONFIGFILEOPERATOR_H

#include <QString>

class ConfigFileOperator
{
private:
    QString projectFilePath;
    QString cellMarkersFilePath;
    QString clusterExpressionFilePath;

public:
    ConfigFileOperator();

    void readConfigFile(QString configFilePath);
    void writeConfigFile(QString configFilePath);

    bool isConfigFileExists(QString configFilePath);

    QString getProjectFilePath();
    QString getCellMarkersFilePath();
    QString getClusterExpressionFilePath();
};

#endif // CONFIGFILEOPERATOR_H
