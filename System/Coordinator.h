#ifndef COORDINATOR_H
#define COORDINATOR_H

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

    QFutureWatcher<QVector<FeatureCollection>> parsingThreadWatcher;

public:
    Coordinator(InformationCenter informationCenter);

signals:
    void finishedCellMarkerFileParsing();
    void finishedClusterFileParsing(QString name);

public slots:
    // ################### INTERACTION WITH START DIALOG ########################
    void on_newProjectStarted(QStringList filePaths);
    void on_filesUploaded(QStringList filePaths);
    void on_projectFileUploaded(QStringList filePaths);
    // ################### INTERACTION WITH START DIALOG ########################

    // ######################### FILE PROCESSING ################################
    void on_datasetFileParsed();
    // ######################### FILE PROCESSING ################################
};

#endif // COORDINATOR_H
