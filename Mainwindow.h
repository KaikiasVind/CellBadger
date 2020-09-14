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
    void on_receivedExpressionDataFromTabWidgets(const QVector<FeatureCollection> clustersWithGeneExpressions);
    void on_filesParsed(const InformationCenter & informationCenter);
    void on_correlatingFinished(const InformationCenter & informationCenter);
    void on_newMaxValuesFound(const double highestMetRawCount, const double highestMetFoldChange, const int numberOfClusters);

signals:
    void newDatasetTabCreated(const QString datasetName, const QVector<QVector<QPair<QString, double>>> correlation);
    void requestGeneExpressionData();
    void runAnalysis(QVector<QVector<FeatureCollection>> allClustersFromAllDatasetsWithGeneExpressions);

    void minRawCountChanged(int minRawCount);
    void maxRawCountChanged(int minRawCount);
    void minfoldChangeChanged(int minFoldChange);
    void maxfoldChangeChanged(int minFoldChange);
    void rawCountInAtLeastChanged(int number);
    void foldChangeInAtLeastChanged(int number);
    void rawCountInAtLeastToggled(bool state);
    void foldChangeInAtLeastToggled(bool state);

private slots:
    __attribute__((noreturn)) void on_buttonExit_clicked();

    void on_buttonMaximize_clicked();

    void on_buttonMinimize_clicked();

    void on_tabWidgetDatasets_currentChanged(int index);

    void on_pushButtonCorrelationOptionsRun_clicked();

    // MIN RAW COUNT
    void on_spinBoxFilterOptionsRawCountCutOffMin_valueChanged(int value);
    void on_horizontalSliderFilterOptionsRawCountCutOffMin_sliderMoved(int position);

    // MAX RAW COUNT
    void on_spinBoxFilterOptionsRawCountCutOffMax_valueChanged(int value);
    void on_horizontalSliderFilterOptionsRawCountCutOffMax_sliderMoved(int position);

    // MIN FOLD CHANGE
    void on_spinBoxFilterOptionsFoldChangeCutOffMin_valueChanged(int value);
    void on_horizontalSliderFilterOptionsFoldChangeCutOffMin_sliderMoved(int position);

    // MAX FOLD CHANGE
    void on_spinBoxFilterOptionsFoldChangeCutOffMax_valueChanged(int value);
    void on_horizontalSliderFilterOptionsFoldChangeCutOffMax_sliderMoved(int position);

    // IN AT LEAST
    void on_checkBoxFilterOptionsRawCountCutOffInAtLeast_toggled(bool checked);
    void on_checkBoxFilterOptionsFoldChangeCutOfftInAtLeast_toggled(bool checked);
    void on_spinBoxFilterOptionsRawCountCutOffInAtLeast_valueChanged(int number);
    void on_spinBoxFilterOptionsFoldChangeCutOffInAtLeast_valueChanged(int number);

    void on_radioButtonAnalysisFilterUseTop_clicked();
    void on_radioButtonAnalysisFilterFilterManually_clicked();

private:
    Ui::MainWindow *ui;

    QVector<TabWidget *> runningTabWidgets;
    TabWidget * currentTabWidget;

    void createDatasetItem(const QString datasetName, const QVector<FeatureCollection> geneExpressions, const QStringList completeGeneIDs, const QStringList clusterNames);
    void updateDatasetItemWithCorrelatedValues(const QVector<QVector<QPair<QString, double>>> correlations);
    void updateCurrentTabWidget();

    double minRawCount;
    double maxRawCount;
    double minFoldChange;
    double maxFoldChange;
    int rawCountinAtLeast;
    bool includeRawCountInAtLeast;
    int foldChangeInAtLeast;
    bool includeFoldChangeInAtLeast;
    double highestMetRawCount;
    double highestMetFoldChange;
    int highestMetNumberOfClusters;

    bool isTabWidgetInitialized = false;


    // Mouse interaction - Necessary for frameless windows
//    void mousePressEvent(QMouseEvent * mousePressEvent);
//    void mouseMoveEvent(QMouseEvent * mouseMoveEvent);
//    int mouseClickXCoordinate;
//    int mouseClickYCoordinate;
};
#endif // MAINWINDOW_H
