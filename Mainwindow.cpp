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
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remove default window frame
//    this->setWindowFlags(Qt::FramelessWindowHint);

    // Set default maximized state
    this->setWindowState(Qt::WindowMaximized);

    // Remove the additional tab that is shown by default on tabwidgets
    this->ui->tabWidgetDatasets->removeTab(0);

    // Disable the "in at least n clusters" spinboxes -> Changed by the radio button in front
    this->ui->spinBoxCorrelationOptionsRawCountCutOffInAtLeast->setDisabled(true);
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffInAtLeast->setDisabled(true);
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
void MainWindow::createDatasetItem(QString datasetName, QVector<FeatureCollection> geneExpressions, const QStringList completeGeneIDs) {
    TabWidget * tabWidget = new TabWidget(this, datasetName);
    QObject::connect(tabWidget, &TabWidget::highestMetRawCountAndFoldChangeValuesChanged, this, &MainWindow::on_highestRawCountAndFoldChangeValuesFound);
    this->runningTabWidgets.append(tabWidget);

    this->ui->tabWidgetDatasets->insertTab(0, tabWidget, datasetName);
    this->ui->tabWidgetDatasets->setCurrentIndex(0);

    this->ui->spinBoxCorrelationOptionsRawCountCutOffInAtLeast->setMaximum(geneExpressions.length());
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffInAtLeast->setMaximum(geneExpressions.length());

    // Forward the gene expression values to the new tab
    tabWidget->populateTableGeneExpressions(geneExpressions, completeGeneIDs);

    // Connect the signals of the main Window to the slots of the TabWidget
    QObject::connect(this, &MainWindow::minRawCountChanged, tabWidget, &TabWidget::on_minRawCountSet);
    QObject::connect(this, &MainWindow::maxRawCountChanged, tabWidget, &TabWidget::on_maxRawCountSet);
    QObject::connect(this, &MainWindow::minfoldChangeChanged, tabWidget, &TabWidget::on_minFoldChangeSet);
    QObject::connect(this, &MainWindow::maxfoldChangeChanged, tabWidget, &TabWidget::on_maxFoldChangeSet);
    QObject::connect(this, &MainWindow::rawCountInAtLeastChanged, tabWidget, &TabWidget::on_rawCountInAtLeastSet);
    QObject::connect(this, &MainWindow::rawCountInAtLeastToggled, tabWidget, &TabWidget::on_rawCountInAtLeastToggled);
    QObject::connect(this, &MainWindow::foldChangeInAtLeastChanged, tabWidget, &TabWidget::on_foldChangeinAtLeastSet);
    QObject::connect(this, &MainWindow::foldChangeInAtLeastToggled, tabWidget, &TabWidget::on_foldChangeInAtLeastToggled);
    QObject::connect(this, &MainWindow::requestGeneExpressionData, tabWidget, &TabWidget::on_runAnalysisRequested);
    QObject::connect(tabWidget, &TabWidget::expressionDataGathered, this, &MainWindow::on_receivedExpressionDataFromTabWidgets);
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
    qDebug() << "Received signal after correlation finished.";

    ui->labelStatus->setText("Finished parsing.");

    QStringList datasetNames;
    datasetNames.reserve(informationCenter.datasetFilePaths.length());

    // Get file names for tab titletab titles
    std::transform(informationCenter.datasetFilePaths.begin(), informationCenter.datasetFilePaths.end(), std::back_inserter(datasetNames), Helper::chopFileName);

    for (int i = 0; i < informationCenter.xClusterCollections.length(); i++) {
        this->createDatasetItem(datasetNames.at(i), informationCenter.xClusterCollections.at(i), informationCenter.completeSetsOfGeneIDsPerDataset.at(i));
    }
}


void MainWindow::on_correlatingFinished(const InformationCenter & informationCenter) {
    qDebug() << "Received signal after correlation finished.";

    this->ui->labelStatus->setText("Correlation finished.");

    for (int i = 0; i < informationCenter.correlatedDatasets.length(); i++) {
        this->runningTabWidgets[i]->populateTableTypeCorrelations(informationCenter.correlatedDatasets.at(i), 5);
    }
}


/**
 * @brief MainWindow::on_expressionValuesChanged
 * @param informationCenter
 */
void MainWindow::on_expressionValuesChanged(const InformationCenter & informationCenter) {
    qDebug() << "Received signal that expression values changed.";
    for (int i = 0; i < informationCenter.xClusterCollections.length(); i++) {
        this->runningTabWidgets[i]->populateTableGeneExpressions(informationCenter.xClusterCollections.at(i), informationCenter.completeSetsOfGeneIDsPerDataset.at(i));
    }
}


void MainWindow::on_tabWidgetDatasets_currentChanged(int index) {
    if (index == this->ui->tabWidgetDatasets->count() - 1 && !this->isHidden()) {
        qDebug() << "Trigger upload new dataset";
    }
}


void MainWindow::on_pushButtonCorrelationOptionsRun_clicked() {
    this->ui->labelStatus->setText("Running correlation...");
    emit this->requestGeneExpressionData();
}


void MainWindow::on_receivedExpressionDataFromTabWidgets(QVector<FeatureCollection> clustersWithGeneExpressions) {
    QVector<QVector<FeatureCollection>> allClustersFromAllDatasetsWithGeneExpressions;
    allClustersFromAllDatasetsWithGeneExpressions << clustersWithGeneExpressions;
    emit this->runAnalysis(allClustersFromAllDatasetsWithGeneExpressions);
}


// ++++++++++++++++++++++++++++++++ MOUSE ++++++++++++++++++++++++++++++++
void MainWindow::mousePressEvent(QMouseEvent * mousePressEvent) {
    this->mouseClickXCoordinate = mousePressEvent->x();
    this->mouseClickYCoordinate = mousePressEvent->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent * mouseMoveEvent) {
    this->move(mouseMoveEvent->globalX() - this->mouseClickXCoordinate, mouseMoveEvent->globalY() - this->mouseClickYCoordinate);
}

void MainWindow::on_spinBoxCorrelationOptionsRawCountCutOffMin_valueChanged(int value) {
    this->ui->horizontalSliderCorrelationOptionsRawCountCutOffMin->setValue(value);
    emit minRawCountChanged(value);
}

void MainWindow::on_horizontalSliderCorrelationOptionsRawCountCutOffMin_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsRawCountCutOffMin->setValue(value);
}

void MainWindow::on_spinBoxCorrelationOptionsRawCountCutOffMax_valueChanged(int value) {
    this->ui->horizontalSliderCorrelationOptionsRawCountCutOffMax->setValue(value);
    emit maxRawCountChanged(value);
}

void MainWindow::on_horizontalSliderCorrelationOptionsRawCountCutOffMax_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsRawCountCutOffMax->setValue(value);
}

void MainWindow::on_checkBoxCorrelationOptionsRawCountCutOffInAtLeast_toggled(bool checked) {
    this->ui->spinBoxCorrelationOptionsRawCountCutOffInAtLeast->setDisabled(!checked);
    emit rawCountInAtLeastToggled(!checked);
}

void MainWindow::on_spinBoxCorrelationOptionsRawCountCutOffInAtLeast_valueChanged(int number) {
    emit rawCountInAtLeastChanged(number);
}

void MainWindow::on_spinBoxCorrelationOptionsFoldChangeCutOffMin_valueChanged(int value) {
    this->ui->horizontalSliderCorrelationOptionsFoldChangeCutOffMin->setValue(value);
    emit minfoldChangeChanged(value);
}

void MainWindow::on_horizontalSliderCorrelationOptionsFoldChangeCutOffMin_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffMin->setValue(value);
}

void MainWindow::on_spinBoxCorrelationOptionsFoldChangeCutOffMax_valueChanged(int value) {
    this->ui->horizontalSliderCorrelationOptionsFoldChangeCutOffMax->setValue(value);
    emit maxfoldChangeChanged(value);
}

void MainWindow::on_horizontalSliderCorrelationOptionsFoldChangeCutOffMax_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffMax->setValue(value);
}

void MainWindow::on_checkBoxCorrelationOptionsFoldChangeCutOfftInAtLeast_toggled(bool checked) {
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffInAtLeast->setDisabled(!checked);
    emit foldChangeInAtLeastToggled(!checked);
}

void MainWindow::on_spinBoxCorrelationOptionsFoldChangeCutOffInAtLeast_valueChanged(int number) {
    emit foldChangeInAtLeastChanged(number);
}

void MainWindow::on_highestRawCountAndFoldChangeValuesFound(const double highestMetRawCount, const double highestMetFoldChange) {
    this->ui->spinBoxCorrelationOptionsRawCountCutOffMin->setMaximum(highestMetRawCount);
    this->ui->spinBoxCorrelationOptionsRawCountCutOffMax->setMaximum(highestMetRawCount);
    this->ui->horizontalSliderCorrelationOptionsRawCountCutOffMin->setMaximum(highestMetRawCount);
    this->ui->horizontalSliderCorrelationOptionsRawCountCutOffMax->setMaximum(highestMetRawCount);
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffMin->setMaximum(highestMetFoldChange);
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffMax->setMaximum(highestMetFoldChange);
    this->ui->horizontalSliderCorrelationOptionsFoldChangeCutOffMin->setMaximum(highestMetFoldChange);
    this->ui->horizontalSliderCorrelationOptionsFoldChangeCutOffMax->setMaximum(highestMetFoldChange);
    this->ui->horizontalSliderCorrelationOptionsRawCountCutOffMax->setValue(highestMetRawCount);
    this->ui->horizontalSliderCorrelationOptionsFoldChangeCutOffMax->setValue(highestMetFoldChange);
    emit this->maxRawCountChanged(highestMetRawCount);
    emit this->maxfoldChangeChanged(highestMetFoldChange);
}
