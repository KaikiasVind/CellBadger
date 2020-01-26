#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QPair>
#include <QString>

#include "StartDialog.h"
#include "System/InformationCenter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//    void plotHeatMap(QVector<QVector<QPair<QString, double>>> tissueCorrelations);
//    void plotHeatMap();

public slots:
    void on_newProjectStarted(QString markerFilePath, QStringList datasetFilePaths);
    void on_clusterFileParsed();
//    void on_correlatingFinished(QVector<QVector<QVector<QPair<QString, double>>>> correlatedDatasets);
    void on_correlatingFinished(InformationCenter informationCenter);

signals:
    void filesUploaded(QStringList filePaths);
    void projectFileUploaded(QStringList filePath);

private slots:
    __attribute__((noreturn)) void on_buttonExit_clicked();

    void on_buttonMaximize_clicked();

    void on_buttonMinimize_clicked();

    void on_tabWidgetDatasets_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QVector<QThread> workingThreads;

    void createDatasetItem(QString datasetName, QVector<QVector<QPair<QString, double>>> correlations);
};
#endif // MAINWINDOW_H
