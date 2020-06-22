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
    this->setWindowFlags(Qt::FramelessWindowHint);

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
    this->runningTabWidgets.append(tabWidget);

    this->ui->tabWidgetDatasets->insertTab(0, tabWidget, datasetName);
    this->ui->tabWidgetDatasets->setCurrentIndex(0);

    // Call the given tab widget to populate its table widgets with the given correlations.
    // The number stands for the top n most correlated types to be shown in the correlation table widget
//    tabWidget->populateTableTypeCorrelations(correlations, 5);
    tabWidget->populateTableGeneExpressions(geneExpressions, completeGeneIDs);
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
    emit runAnalysis();
}


// RAW COUNT CUT-OFF
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
}

void MainWindow::on_horizontalSliderCorrelationOptionsRawCountCutOffMin_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsRawCountCutOffMin->setValue(value);
}

void MainWindow::on_spinBoxCorrelationOptionsRawCountCutOffMax_valueChanged(int value) {
    this->ui->horizontalSliderCorrelationOptionsRawCountCutOffMax->setValue(value);
}

void MainWindow::on_horizontalSliderCorrelationOptionsRawCountCutOffMax_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsRawCountCutOffMax->setValue(value);
}

void MainWindow::on_checkBoxCorrelationOptionsRawCountCutOffInAtLeast_toggled(bool checked) {
    this->ui->spinBoxCorrelationOptionsRawCountCutOffInAtLeast->setDisabled(!checked);
}

void MainWindow::on_spinBoxCorrelationOptionsFoldChangeCutOffMin_valueChanged(int value) {
    this->ui->horizontalSliderCorrelationOptionsFoldChangeCutOffMin->setValue(value);
}

void MainWindow::on_horizontalSliderCorrelationOptionsFoldChangeCutOffMin_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffMin->setValue(value);
}

void MainWindow::on_spinBoxCorrelationOptionsFoldChangeCutOffMax_valueChanged(int value) {
    this->ui->horizontalSliderCorrelationOptionsFoldChangeCutOffMax->setValue(value);
}

void MainWindow::on_horizontalSliderCorrelationOptionsFoldChangeCutOffMax_valueChanged(int value) {
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffMax->setValue(value);
}

void MainWindow::on_checkBoxCorrelationOptionsFoldChangeCutOfftInAtLeast_toggled(bool checked) {
    this->ui->spinBoxCorrelationOptionsFoldChangeCutOffInAtLeast->setDisabled(!checked);
}
