#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <QFutureSynchronizer>
#include <QFutureWatcher>
#include <QObject>
#include <QString>
#include <QStringList>

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
    QFutureSynchronizer<QVector<QVector<QPair<QString, double>>>> correlatorThreadsWatcher;

    void parseDatasetFiles(const QStringList datasetFilePaths);

    void printResults(); //REMEBER: DELETE ME!!!!

    template<typename F>
    void parseFiles(const QStringList filePaths, const F & parsingFunction, const double cutoff);
    void saveInformationAfterParsingFinished();
    void correlateDatasets(const QVector<QVector<FeatureCollection>> xClusterDatasets, const QVector<FeatureCollection> cellMarkersForTypes);
    void saveInformationAfterCorrelatingFinished();

public:
    Coordinator(InformationCenter informationCenter);

signals:
    void finishedFileParsing();
    void finishedCellMarkerFileParsing();
    void finishedClusterFilesParsing();
    void finishedCorrelating(const InformationCenter informationCenter);

public slots:
    // ################### INTERACTION WITH START DIALOG ########################
    void on_newProjectStarted(const QString cellMarkerFilePath, const QStringList filePaths);
    void on_filesUploaded(const QStringList filePaths);
    void on_projectFileUploaded(const QStringList filePaths);
    // ################### INTERACTION WITH START DIALOG ########################

    // ######################### FILE PROCESSING ################################
    // ######################### FILE PROCESSING ################################
};

#endif // COORDINATOR_H
