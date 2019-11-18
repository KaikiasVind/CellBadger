#include "ConfigFileOperator.h"

#include <QString>
#include <QFile>
#include <QByteArray>
#include <QList>
#include <QVector>
#include <QDebug>

ConfigFileOperator::ConfigFileOperator(){}

/**
 * @brief ConfigFileOperator::isConfigFileExists
 * @param configFilePath
 * @return
 */
bool ConfigFileOperator::isConfigFileExists(QString configFilePath) {
    return QFile::exists(configFilePath);
}


/**
 * @brief ConfigFileOperator::getProjectFilePath
 * @return
 */
QString ConfigFileOperator::getProjectFilePath() {
    return projectFilePath;
}


/**
 * @brief ConfigFileOperator::getCellMarkersFilePath
 * @return
 */
QString ConfigFileOperator::getCellMarkersFilePath() {
    return cellMarkersFilePath;
}


/**
 * @brief ConfigFileOperator::getCellMarkersFilePath
 * @return
 */
QString ConfigFileOperator::getClusterExpressionFilePath() {
    return clusterExpressionFilePath;
}


/**
 * @brief ConfigFileOperator::readConfigFile
 * @param configFilePath
 */
void ConfigFileOperator::readConfigFile(QString configFilePath) {
    // Open file
    QFile csvFile(configFilePath);

    // Throw error in case opening the file fails
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << "CONFIG FILE:" << csvFile.errorString();
        exit(1);
    }

    // Skip title line
    QByteArray line;
    QList<QByteArray> splitLine;

    // Config identifiers
    QString projectLocation       = "project_location",
            markerFile            = "marker_file",
            clusterExpressionFile = "cluster_expression_file";

    // Start parsing cluster file
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        splitLine = line.split('=');

        // Split up IDENTIFIER=VALUE
        QString identifier = splitLine.first(),
                     value = splitLine.last();

        // Remove linebreak character
        value.chop(1);

        // Fill config variables with values from file
        if (identifier == projectLocation)
            configFilePath = value;
        else if (identifier == markerFile)
            cellMarkersFilePath = value;
        else if (identifier == clusterExpressionFile)
            clusterExpressionFilePath = value;
    }
}
