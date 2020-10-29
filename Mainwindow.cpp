#include "Mainwindow.h"
#include "ui_Mainwindow.h"

#include <QVector>
#include <QPair>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QListView>
#include <QTableWidget>
#include <QMouseEvent>
#include <math.h>

#include "StartDialog.h"
#include "TabWidget.h"
#include "Utils/Helper.h"
#include "System/InformationCenter.h"
#include "BioModels/FeatureCollection.h"
#include "Utils/Definitions.h"

using Definitions::AnalysisFilterMode;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remove default window frame
//    this->setWindowFlags(Qt::FramelessWindowHint);

    // Set default maximized state
    this->setWindowState(Qt::WindowMaximized);

    // Remove the additional tab that is shown by default on tabwidgets
    this->ui->tabWidgetDatasets->removeTab(0);

    // Create the Analysis Tab
    this->analysisTab = new AnalysisTab(this);

    // Add the signal-signal connections of the MainWindow and the AnalysisTab. The MainWindow just serves as a middle-man between AnalysisTab and Coordinator
    QObject::connect(this->analysisTab, &AnalysisTab::requestGeneExpressionData, this, &MainWindow::requestGeneExpressionDataForAnalysisTab);
    QObject::connect(this, &MainWindow::transmitGeneExpressionDataForAnalysisTab, this->analysisTab, &AnalysisTab::on_receivedGeneExpressionData);

    // Insert the AnalysisTab and disable it until after the analysis
    this->ui->tabWidgetDatasets->insertTab(0, this->analysisTab, "Analysis");
    this->ui->tabWidgetDatasets->setTabEnabled(0, false);

    this->includeRawCountInAtLeast = this->ui->checkBoxFilterOptionsRawCountCutOffInAtLeast->isChecked();
    this->includeFoldChangeInAtLeast = this->ui->checkBoxFilterOptionsFoldChangeCutOfftInAtLeast->isChecked();
    this->includeRawCountInAtLeast = this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->value();
    this->includeFoldChangeInAtLeast = this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->value();
    this->highestMetNumberOfClusters = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * @brief MainWindow::createDatasetItem - Generates a new tab and calls the new tab to populate its table-widgets
 * @param datasetName - File name of the given dataset
 * @param correlations - List of clusters with corresponding correlated types
 */
void MainWindow::createDatasetItem(QString datasetName, QVector<FeatureCollection> geneExpressions, const QStringList completeGeneIDs, const QStringList clusterNames) {
    TabWidget * tabWidget = new TabWidget(this, datasetName, clusterNames);

    // Connect to the tabwidget to get the max values that are going to be found when populating the gene expression table below
    QObject::connect(tabWidget, &TabWidget::maxValuesFound, this, &MainWindow::on_newMaxValuesFound);

    this->runningTabWidgets.append(tabWidget);

    this->ui->tabWidgetDatasets->insertTab(this->runningTabWidgets.length() - 1, tabWidget, datasetName);
//    this->ui->tabWidgetDatasets->insertTab(0, tabWidget, datasetName);
    this->ui->tabWidgetDatasets->setCurrentIndex(0);
    this->currentTabWidget = this->runningTabWidgets.at(0);

    // Forward the gene expression values to the new tab
    tabWidget->populateTableGeneExpressions(geneExpressions, completeGeneIDs);

    // Report that the TabWidget has been initialized now -> Necessary for updating purposes
    this->isTabWidgetInitialized = true;
}


/**
 * @brief MainWindow::updateCurrentTabWidget - Grab all values from the MainWindow and send them to the current TabWidget
 */
void MainWindow::updateCurrentTabWidget() {
    // Update RPM
    this->currentTabWidget->setMinRPM(this->minRPM);
    this->currentTabWidget->setMaxRPM(this->maxRPM);
    this->currentTabWidget->setIncludeRPMInAtLeast(this->includeRPMInAtLeast);
    this->currentTabWidget->setRPMInAtLeast(this->rpmInAtLeast);

    // Update Raw Count
    this->currentTabWidget->setMinRawCount(this->minRawCount);
    this->currentTabWidget->setMaxRawCount(this->maxRawCount);
    this->currentTabWidget->setIncludeRawCountInAtLeast(this->includeRawCountInAtLeast);
    this->currentTabWidget->setRawCountInAtLeast(this->rawCountinAtLeast);

    // Update Fold Change
    this->currentTabWidget->setMinFoldChange(this->minFoldChange);
    this->currentTabWidget->setMaxFoldChange(this->maxFoldChange);
    this->currentTabWidget->setIncludeFoldChangeInAtLeast(this->includeFoldChangeInAtLeast);
    this->currentTabWidget->setFoldChangeInAtLeast(this->foldChangeInAtLeast);
}

// ############################################### SLOTS ###############################################

// REACTING TO CONTROLLER
void MainWindow::on_filesParsed(const InformationCenter & informationCenter) {
    qDebug() << "Received signal after parsing finished.";
    ui->labelStatus->setText("Finished parsing.");

    // Show the window on first show
    if (this->isHidden())
        this->show();

    // Get file names for tab titletab titles
    QStringList datasetNames = Helper::getFileNames(informationCenter.datasetFilePaths);

    // Go through every parsed dataset and add a new tab and analysis-tab entry for it
    for (int i = 0; i < informationCenter.xClusterCollections.length(); i++) {
        this->createDatasetItem(datasetNames.at(i), informationCenter.xClusterCollections.at(i), informationCenter.completeSetsOfGeneIDsPerDataset.at(i), informationCenter.clusterNamesForDatasets.at(i));
    }
}


/**
 * @brief MainWindow::on_correlatingFinished - Transfers the data given by the coordinator to the corresponding TabWidgets
 * @param informationCenter - The core information structure containing e.g. the correllation analysis data
 */
void MainWindow::on_correlatingFinished(const InformationCenter & informationCenter) {
    this->ui->labelStatus->setText("Finished correlation.");

    // Remove all previously added experiment-data of the analysis tab
    this->analysisTab->cleanTable();

    // Get file names for tab titletab titles
    QStringList datasetNames = Helper::getFileNames(informationCenter.datasetFilePaths);

    for (int i = 0; i < informationCenter.correlatedDatasets.length(); i++) {
        this->runningTabWidgets[i]->populateTableTypeCorrelations(informationCenter.correlatedDatasets.at(i), informationCenter.qualityScores.at(i), 5);
        this->analysisTab->addExperiment(datasetNames.at(i), informationCenter.xClusterCollections.at(i), informationCenter.correlatedDatasets.at(i));
    }

    // Check whether the analysis tab is still disabled. If so, enable it
    int analysisTabIndex = this->runningTabWidgets.length();
    if (!this->ui->tabWidgetDatasets->isTabEnabled(analysisTabIndex)) {
        this->ui->tabWidgetDatasets->setTabEnabled(analysisTabIndex, true);
    }

}


void MainWindow::on_tabWidgetDatasets_currentChanged(int index) {

//    if (index == this->ui->tabWidgetDatasets->count() - 1 && !this->isHidden()) {
//        qDebug() << "Trigger upload new dataset";
//    }

    if (this->isHidden() || !this->isTabWidgetInitialized || index > this->runningTabWidgets.length() - 1)
        return;

    // Set the new Tab as the current widget
    this->currentTabWidget = this->runningTabWidgets.at(index);

    // And send all current cut-offs the new widget
    // This causes that the ProxyModel filter is only updated for the TabWidget that is currently seen
    // This lazy evaluation was chosen to reduce the calculation time for the ProxyModel filter in case more than one tab is present
    this->updateCurrentTabWidget();
}


void MainWindow::on_pushButtonCorrelationOptionsRun_clicked() {
    this->ui->labelStatus->setText("Running correlation...");

    int numberOfGenesToUse = this->ui->spinBoxFilterAnalysisFilterUseTop->value();

    emit this->runAnalysis(numberOfGenesToUse);
}


//void MainWindow::on_receivedExpressionDataFromTabWidgets(QVector<FeatureCollection> clustersWithGeneExpressions) {
//    QVector<QVector<FeatureCollection>> allClustersFromAllDatasetsWithGeneExpressions;
//    allClustersFromAllDatasetsWithGeneExpressions << clustersWithGeneExpressions;
//    emit this->runAnalysis(allClustersFromAllDatasetsWithGeneExpressions);
//}


void MainWindow::on_newMaxValuesFound(const double highestMetRawCount, const double highestMetFoldChange, const int numberOfClusters) {

    double ceiledHighestMetRawCount = ceil(highestMetRawCount),
           ceiledHighestMetFoldChange = ceil(highestMetFoldChange);

    // Compare the new max values with the previous max values and update the current values in case any value is higher
    if (this->highestMetRawCount < ceiledHighestMetRawCount) {
        this->highestMetRawCount = ceiledHighestMetRawCount;
        this->ui->spinBoxFilterOptionsRawCountCutOffMin->setMaximum(ceiledHighestMetRawCount);
        this->ui->spinBoxFilterOptionsRawCountCutOffMax->setMaximum(ceiledHighestMetRawCount);
        this->ui->horizontalSliderFilterOptionsRawCountCutOffMin->setMaximum(ceiledHighestMetRawCount);
        this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setMaximum(ceiledHighestMetRawCount);
        this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setValue(ceiledHighestMetRawCount);
        emit this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->sliderMoved(ceiledHighestMetRawCount);
    }

    if (this->highestMetFoldChange < ceiledHighestMetFoldChange) {
        this->highestMetFoldChange = ceiledHighestMetFoldChange;
        this->ui->spinBoxFilterOptionsFoldChangeCutOffMin->setMaximum(10); // > 10 is unrealistic
        this->ui->spinBoxFilterOptionsFoldChangeCutOffMax->setMaximum(ceiledHighestMetFoldChange);
        this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMin->setMaximum(10);
        this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setMaximum(ceiledHighestMetFoldChange);
        this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setValue(ceiledHighestMetFoldChange);
        emit this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->sliderMoved(ceiledHighestMetFoldChange);
    }

    if (this->highestMetNumberOfClusters < numberOfClusters) {
        this->highestMetNumberOfClusters = numberOfClusters;
        this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->setMaximum(numberOfClusters);
        this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->setMaximum(numberOfClusters);
    }
}


// ####################################### TOP GENES #######################################

void MainWindow::on_horizontalSliderFilterOptionsTopGenesSelection_sliderMoved(int position) {
    this->numberOfGenesToUse = position;
    this->ui->spinBoxFilterAnalysisFilterUseTop->setValue(position);
}

void MainWindow::on_spinBoxFilterAnalysisFilterUseTop_valueChanged(int number) {
    this->numberOfGenesToUse = number;
    this->ui->horizontalSliderFilterOptionsTopGenesSelection->setValue(number);
}


// ####################################### RPM #############################################

// MIN RPM
void MainWindow::on_spinBoxFilterOptionsRPMCutOffMin_valueChanged(int value) {
    this->minRPM = value;
    this->ui->horizontalSliderFilterOptionsRPMCutOffMin->setValue(value);
    this->currentTabWidget->setMinRPM(value);
}

void MainWindow::on_horizontalSliderFilterOptionsRPMCutOffMin_sliderMoved(int position) {
    this->minRPM = position;
    this->ui->spinBoxFilterOptionsRPMCutOffMin->setValue(position);
}

// MAX RPM
void MainWindow::on_spinBoxFilterOptionsRPMCutOffMax_valueChanged(int value) {
    this->maxRPM = value;
    this->ui->horizontalSliderFilterOptionsRPMCutOffMax->setValue(value);
    this->currentTabWidget->setMaxRPM(value);
}

void MainWindow::on_horizontalSliderFilterOptionsRPMCutOffMax_sliderMoved(int position) {
    this->maxRPM = position;
    this->ui->spinBoxFilterOptionsRPMCutOffMax->setValue(position);
}

// RPM IN AT LEAST
void MainWindow::on_checkBoxFilterOptionsRPMCutOffInAtLeast_toggled(bool checked) {
    this->includeRPMInAtLeast = checked;
    this->ui->spinBoxFilterOptionsRPMCutOffInAtLeast->setEnabled(checked);
    this->currentTabWidget->setIncludeRPMInAtLeast(checked);
}

void MainWindow::on_spinBoxFilterOptionsRPMCutOffInAtLeast_valueChanged(int number) {
    this->rpmInAtLeast = number;
    this->currentTabWidget->setRPMInAtLeast(number);
}

// ####################################### RAW COUNT #######################################

// MIN RAW COUNT
void MainWindow::on_spinBoxFilterOptionsRawCountCutOffMin_valueChanged(int value) {
    this->minRawCount = value;
    this->ui->horizontalSliderFilterOptionsRawCountCutOffMin->setValue(value);
    this->currentTabWidget->setMinRawCount(value);
}

void MainWindow::on_horizontalSliderFilterOptionsRawCountCutOffMin_sliderMoved(int position) {
    this->minRawCount = position;
    this->ui->spinBoxFilterOptionsRawCountCutOffMin->setValue(position);
}

// MAX RAW COUNT
void MainWindow::on_spinBoxFilterOptionsRawCountCutOffMax_valueChanged(int value) {
    this->maxRawCount = value;
    this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setValue(value);
    this->currentTabWidget->setMaxRawCount(value);
}

void MainWindow::on_horizontalSliderFilterOptionsRawCountCutOffMax_sliderMoved(int position) {
    this->maxRawCount = position;
    this->ui->spinBoxFilterOptionsRawCountCutOffMax->setValue(position);
}

// RAW COUNT IN AT LEAST
void MainWindow::on_checkBoxFilterOptionsRawCountCutOffInAtLeast_toggled(bool checked) {
    this->includeRawCountInAtLeast = checked;
    this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->setEnabled(checked);
    this->currentTabWidget->setIncludeRawCountInAtLeast(checked);
}

void MainWindow::on_spinBoxFilterOptionsRawCountCutOffInAtLeast_valueChanged(int number) {
    this->rawCountinAtLeast = number;
    this->currentTabWidget->setRawCountInAtLeast(number);
}

// ####################################### FOLD CHANGE #######################################

// MIN FOLD CHANGE
void MainWindow::on_spinBoxFilterOptionsFoldChangeCutOffMin_valueChanged(int value) {
    this->minFoldChange = value;
    this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMin->setValue(value);
    this->currentTabWidget->setMinFoldChange(value);
}

void MainWindow::on_horizontalSliderFilterOptionsFoldChangeCutOffMin_sliderMoved(int position) {
    this->minFoldChange = position;
    this->ui->spinBoxFilterOptionsFoldChangeCutOffMin->setValue(position);
}

// MAX FOLD CHANGE
void MainWindow::on_spinBoxFilterOptionsFoldChangeCutOffMax_valueChanged(int value) {
    this->maxFoldChange = value;
    this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setValue(value);
    this->currentTabWidget->setMaxFoldChange(value);
}

void MainWindow::on_horizontalSliderFilterOptionsFoldChangeCutOffMax_sliderMoved(int position) {
    this->maxFoldChange = position;
    this->ui->spinBoxFilterOptionsFoldChangeCutOffMax->setValue(position);
}

// FOLD CHANGE IN AT LEAST
void MainWindow::on_checkBoxFilterOptionsFoldChangeCutOfftInAtLeast_toggled(bool checked) {
    this->includeFoldChangeInAtLeast = checked;
    this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->setEnabled(checked);
    this->currentTabWidget->setIncludeFoldChangeInAtLeast(checked);
}

void MainWindow::on_spinBoxFilterOptionsFoldChangeCutOffInAtLeast_valueChanged(int number) {
    this->foldChangeInAtLeast = number;
    this->currentTabWidget->setFoldChangeInAtLeast(number);
}

