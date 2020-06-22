#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QPair>
#include <QString>
#include <QMouseEvent>
#include <QStringList>

#include "StartDialog.h"
#include "TabWidget.h"
#include "System/InformationCenter.h"
#include "BioModels/FeatureCollection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_filesParsed(const InformationCenter & informationCenter);
    void on_correlatingFinished(const InformationCenter & informationCenter);

signals:
    void newDatasetTabCreated(const QString datasetName, const QVector<QVector<QPair<QString, double>>> correlation);
    void runAnalysis();

private slots:
    __attribute__((noreturn)) void on_buttonExit_clicked();

    void on_buttonMaximize_clicked();

    void on_buttonMinimize_clicked();

    void on_tabWidgetDatasets_currentChanged(int index);

    void on_pushButtonCorrelationOptionsRun_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QThread> workingThreads;

    QVector<TabWidget *> runningTabWidgets;

    void createDatasetItem(const QString datasetName, const QVector<FeatureCollection> geneExpressions, const QStringList completeGeneIDs);
    void updateDatasetItemWithCorrelatedValues(const QVector<QVector<QPair<QString, double>>> correlations);

//    void createDatasetItem(const QString datasetName, const QVector<QVector<QPair<QString, double>>> correlations,
//                           const QVector<FeatureCollection> geneExpressions, const QStringList completeGeneIDs);

    // Mouse interaction - Necessary for frameless windows
    void mousePressEvent(QMouseEvent * mousePressEvent);
    void mouseMoveEvent(QMouseEvent * mouseMoveEvent);
    int mouseClickXCoordinate;
    int mouseClickYCoordinate;
};
#endif // MAINWINDOW_H
