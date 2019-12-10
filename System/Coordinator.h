#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <QFutureSynchronizer>
#include <QFutureWatcher>
#include <QObject>

#include "System/InformationCenter.h"

/**
 * @brief The Coordinator class - This class is used to model the basic workflow and to concentrate the program logic in one place
 */
class Coordinator : public QObject
{
    Q_OBJECT

private:
    InformationCenter informationCenter;

    QFutureSynchronizer<QVector<FeatureCollection>> parsingThreadsWatcher;

    void gatherInformationAfterParsingFinished();

public:
    Coordinator(InformationCenter informationCenter);

signals:
    void finishedFileParsing();
    void finishedCellMarkerFileParsing();
    void finishedClusterFilesParsing();

public slots:
    // ################### INTERACTION WITH START DIALOG ########################
    void on_newProjectStarted(QString cellMarkerFilePath, QStringList filePaths);
    void on_filesUploaded(QStringList filePaths);
    void on_projectFileUploaded(QStringList filePaths);
    // ################### INTERACTION WITH START DIALOG ########################

    // ######################### FILE PROCESSING ################################
    // ######################### FILE PROCESSING ################################
};

#endif // COORDINATOR_H
