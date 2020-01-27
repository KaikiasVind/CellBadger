#include "ConfigFileOperator.h"

#include <QString>
#include <QFile>
#include <QByteArray>
#include <QList>
#include <QVector>
#include <QDebug>

#include "System/ConfigFile.h"


namespace ConfigFileOperator {

/**
 * @brief isConfigFileExists
 * @param configFilePath
 * @return
 */
bool isConfigFileExists(QString configFilePath) {
    return QFile::exists(configFilePath);
}


/**
 * @brief readConfigFile
 * @param configFilePath
 */
ConfigFile readConfigFile(QString configFilePath) {
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

    // Gather information from config file
    QString cellMarkersFilePath,
            clusterExpressionFilePath;

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

    // Assemble config file and return it
    ConfigFile configFile(cellMarkersFilePath, clusterExpressionFilePath);
    return configFile;
}

/**
 * @brief createConfigFile - Creates a txt file for usage later on
 * @param configFilePath - Full file path to config file
 */
void createConfigFile(QString configFilePath) {
//    QDir directory;

//    // Create the directory if it does not exist
//    if (!directory.exists(configFilePath))
//        directory.mkpath(configFilePath);

    if (Q_UNLIKELY(isConfigFileExists(configFilePath)))
        return;

    QFile configFile(configFilePath);
    configFile.open(QIODevice::WriteOnly);
}


/**
 * @brief initializeConfigFile
 * @return
 */
ConfigFile initializeConfigFile() {
    //REMEMBER: A default value should be used for the marker file here
    ConfigFile configFile("nAn", "nAn");
    return configFile;
}


/**
 * @brief isFilesExist - Goes through every given file and returns whether all files exist on the filesystem
 * @param filePaths - List of directory paths for the corresponding files
 * @return - One of true or false. True if EVERY file exists, false if at least one file does not exist.
 */
bool isFileExists(QString filePath) {
    if (!QFile::exists(filePath)) {
        return false;
    }
    return true;
}

}
