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
//    void on_expressionValuesChanged(const InformationCenter & informationCenter);
    void on_highestRawCountAndFoldChangeValuesFound(const double highestMetRawCount, const double highestMetFoldChange);
    void on_receivedExpressionDataFromTabWidgets(const QVector<FeatureCollection> clustersWithGeneExpressions);
    void on_filesParsed(const InformationCenter & informationCenter);
    void on_correlatingFinished(const InformationCenter & informationCenter);

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

    void on_spinBoxCorrelationOptionsRawCountCutOffMin_valueChanged(int value);

    void on_horizontalSliderCorrelationOptionsRawCountCutOffMin_valueChanged(int value);

    void on_spinBoxCorrelationOptionsRawCountCutOffMax_valueChanged(int value);

    void on_horizontalSliderCorrelationOptionsRawCountCutOffMax_valueChanged(int value);

    void on_checkBoxCorrelationOptionsRawCountCutOffInAtLeast_toggled(bool checked);

    void on_spinBoxCorrelationOptionsFoldChangeCutOffMin_valueChanged(int value);

    void on_horizontalSliderCorrelationOptionsFoldChangeCutOffMin_valueChanged(int value);

    void on_spinBoxCorrelationOptionsFoldChangeCutOffMax_valueChanged(int value);

    void on_horizontalSliderCorrelationOptionsFoldChangeCutOffMax_valueChanged(int value);

    void on_checkBoxCorrelationOptionsFoldChangeCutOfftInAtLeast_toggled(bool checked);

    void on_spinBoxCorrelationOptionsRawCountCutOffInAtLeast_valueChanged(int number);

    void on_spinBoxCorrelationOptionsFoldChangeCutOffInAtLeast_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;

    QVector<TabWidget *> runningTabWidgets;

    void createDatasetItem(const QString datasetName, const QVector<FeatureCollection> geneExpressions, const QStringList completeGeneIDs);
    void updateDatasetItemWithCorrelatedValues(const QVector<QVector<QPair<QString, double>>> correlations);

    // Mouse interaction - Necessary for frameless windows
//    void mousePressEvent(QMouseEvent * mousePressEvent);
//    void mouseMoveEvent(QMouseEvent * mouseMoveEvent);
//    int mouseClickXCoordinate;
//    int mouseClickYCoordinate;
};
#endif // MAINWINDOW_H
