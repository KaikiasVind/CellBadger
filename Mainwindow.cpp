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

#include "StartDialog.h"
#include "TabWidget.h"
#include "Utils/Helper.h"
#include "System/InformationCenter.h"
#include "BioModels/FeatureCollection.h"

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

    // Disable the "in at least n clusters" spinboxes -> Changed by the radio button in front
    this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->setDisabled(true);
    this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->setDisabled(true);

    this->includeRawCountInAtLeast = this->ui->checkBoxFilterOptionsRawCountCutOffInAtLeast->isChecked();
    this->includeFoldChangeInAtLeast = this->ui->checkBoxFilterOptionsFoldChangeCutOfftInAtLeast->isChecked();
    this->includeRawCountInAtLeast = this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->value();
    this->includeFoldChangeInAtLeast = this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->value();
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

    this->ui->tabWidgetDatasets->insertTab(this->runningTabWidgets.length(), tabWidget, datasetName);
    this->ui->tabWidgetDatasets->setCurrentIndex(0);
    this->currentTabWidget = this->runningTabWidgets.at(0);

    // Forward the gene expression values to the new tab
    tabWidget->populateTableGeneExpressions(geneExpressions, completeGeneIDs);

    // Report that the TabWidget has been initialized now -> Necessary for updating purposes
    this->isTabWidgetInitialized = true;
}


// ############################################### SLOTS ###############################################
/**
 * @brief MainWindow::on_buttonExit_clicked - Shutdown the program
 */
__attribute__((noreturn)) void MainWindow::on_buttonExit_clicked() {
    qDebug() << "Exiting";
    exit(0);
}

/**
 * @brief MainWindow::on_buttonMaximize_clicked - Minimises the main window
 */
void MainWindow::on_buttonMaximize_clicked() {
    // Allow maximize and unmaximize
    if (this->windowState() == Qt::WindowMaximized) {
        this->setWindowState(Qt::WindowNoState);
    } else {
        this->setWindowState(Qt::WindowMaximized);
    }
}

/**
 * @brief MainWindow::on_buttonMinimize_clicked - Maximises the main window
 */
void MainWindow::on_buttonMinimize_clicked() {
    this->setWindowState(Qt::WindowMinimized);
}


// REACTING TO CONTROLLER
void MainWindow::on_filesParsed(const InformationCenter & informationCenter) {
    this->show();
    qDebug() << "Received signal after parsing finished.";

    ui->labelStatus->setText("Finished parsing.");

    QStringList datasetNames;
    datasetNames.reserve(informationCenter.datasetFilePaths.length());

    // Get file names for tab titletab titles
    std::transform(informationCenter.datasetFilePaths.begin(), informationCenter.datasetFilePaths.end(), std::back_inserter(datasetNames), Helper::chopFileName);

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

    for (int i = 0; i < informationCenter.correlatedDatasets.length(); i++) {
        this->runningTabWidgets[i]->populateTableTypeCorrelations(informationCenter.correlatedDatasets.at(i), informationCenter.qualityScores.at(i), 5);
    }
}


void MainWindow::on_tabWidgetDatasets_currentChanged(int index) {


//    if (index == this->ui->tabWidgetDatasets->count() - 1 && !this->isHidden()) {
//        qDebug() << "Trigger upload new dataset";
//    }

    if (this->isHidden() || !this->isTabWidgetInitialized)
        return;

    // Set the new Tab as the current widget
    this->currentTabWidget = this->runningTabWidgets.at(index);

    // And send all current cut-offs the new widget
    // This causes that the ProxyModel filter is only updated for the TabWidget that is currently seen
    // This lazy evaluation was chosen to reduce the calculation time for the ProxyModel filter in case more than one tab is present
    this->currentTabWidget->setMinRawCount(this->minRawCount);
    this->currentTabWidget->setMaxRawCount(this->maxRawCount);
    this->currentTabWidget->setMinFoldChange(this->minFoldChange);
    this->currentTabWidget->setMaxFoldChange(this->maxFoldChange);
    this->currentTabWidget->setIncludeRawCountInAtLeast(this->includeRawCountInAtLeast);
    this->currentTabWidget->setIncludeFoldChangeInAtLeast(this->includeFoldChangeInAtLeast);
    this->currentTabWidget->setRawCountInAtLeast(this->rawCountinAtLeast);
    this->currentTabWidget->setFoldChangeInAtLeast(this->foldChangeInAtLeast);
}


void MainWindow::on_pushButtonCorrelationOptionsRun_clicked() {
    this->ui->labelStatus->setText("Running correlation...");
    QVector<QVector<FeatureCollection>> allClustersFromAllDatasetsWithGeneExpressions;
    for (TabWidget * tabWidget : this->runningTabWidgets)
        allClustersFromAllDatasetsWithGeneExpressions.append(tabWidget->retrieveAllSeenData());
    emit this->runAnalysis(allClustersFromAllDatasetsWithGeneExpressions);
}


void MainWindow::on_receivedExpressionDataFromTabWidgets(QVector<FeatureCollection> clustersWithGeneExpressions) {
    QVector<QVector<FeatureCollection>> allClustersFromAllDatasetsWithGeneExpressions;
    allClustersFromAllDatasetsWithGeneExpressions << clustersWithGeneExpressions;
    emit this->runAnalysis(allClustersFromAllDatasetsWithGeneExpressions);
}


void MainWindow::on_newMaxValuesFound(const double highestMetRawCount, const double highestMetFoldChange, const int numberOfClusters) {

    // Compare the new max values with the previous max values and update the current values in case any value is higher
    if (this->highestMetRawCount < highestMetRawCount) {
        this->highestMetRawCount = highestMetRawCount;
        this->ui->spinBoxFilterOptionsRawCountCutOffMin->setMaximum(highestMetRawCount);
        this->ui->spinBoxFilterOptionsRawCountCutOffMax->setMaximum(highestMetRawCount);
        this->ui->horizontalSliderFilterOptionsRawCountCutOffMin->setMaximum(highestMetRawCount);
        this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setMaximum(highestMetRawCount);
        this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setValue(highestMetRawCount);
        emit this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->sliderMoved(highestMetRawCount);
    }

    if (this->highestMetFoldChange < highestMetFoldChange) {
        this->highestMetFoldChange = highestMetFoldChange;
        this->ui->spinBoxFilterOptionsFoldChangeCutOffMin->setMaximum(highestMetFoldChange);
        this->ui->spinBoxFilterOptionsFoldChangeCutOffMax->setMaximum(highestMetFoldChange);
        this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMin->setMaximum(highestMetFoldChange);
        this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setMaximum(highestMetFoldChange);
        this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setValue(highestMetFoldChange);
        emit this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->sliderMoved(highestMetFoldChange);
    }

    if (this->highestMetNumberOfClusters < numberOfClusters) {
        this->highestMetNumberOfClusters = numberOfClusters;
        this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->setMaximum(numberOfClusters);
        this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->setMaximum(numberOfClusters);
    }
}


// GUI

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


// ++++++++++++++++++++++++++++++++ MOUSE ++++++++++++++++++++++++++++++++
//void MainWindow::mousePressEvent(QMouseEvent * mousePressEvent) {
//    this->mouseClickXCoordinate = mousePressEvent->x();
//    this->mouseClickYCoordinate = mousePressEvent->y();
//}

//void MainWindow::mouseMoveEvent(QMouseEvent * mouseMoveEvent) {
//    this->move(mouseMoveEvent->globalX() - this->mouseClickXCoordinate, mouseMoveEvent->globalY() - this->mouseClickYCoordinate);
//}
