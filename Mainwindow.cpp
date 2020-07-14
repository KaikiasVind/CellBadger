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
    this->runningTabWidgets.append(tabWidget);

    this->ui->tabWidgetDatasets->insertTab(this->runningTabWidgets.length(), tabWidget, datasetName);
    this->ui->tabWidgetDatasets->setCurrentIndex(0);

    // Forward the gene expression values to the new tab
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
        this->createDatasetItem(datasetNames.at(i), informationCenter.xClusterCollections.at(i), informationCenter.completeSetsOfGeneIDsPerDataset.at(i), informationCenter.clusterNamesForDatasets.at(i));
    }
}


/**
 * @brief MainWindow::on_correlatingFinished - Transfers the data given by the coordinator to the corresponding TabWidgets
 * @param informationCenter - The core information structure containing e.g. the correllation analysis data
 */
void MainWindow::on_correlatingFinished(const InformationCenter & informationCenter) {
    qDebug() << "Received signal after correlation finished.";

    this->ui->labelStatus->setText("Correlation finished.");

    for (int i = 0; i < informationCenter.correlatedDatasets.length(); i++) {
        this->runningTabWidgets[i]->populateTableTypeCorrelations(informationCenter.correlatedDatasets.at(i), informationCenter.qualityScores.at(i), 5);
    }
}


/**
 * @brief MainWindow::on_expressionValuesChanged
 * @param informationCenter
 */
//void MainWindow::on_expressionValuesChanged(const InformationCenter & informationCenter) {
//    qDebug() << "Received signal that expression values changed.";
//    for (int i = 0; i < informationCenter.xClusterCollections.length(); i++) {
//        this->runningTabWidgets[i]->populateTableGeneExpressions(informationCenter.xClusterCollections.at(i), informationCenter.completeSetsOfGeneIDsPerDataset.at(i));
//    }
//}


void MainWindow::on_tabWidgetDatasets_currentChanged(int index) {
    if (index == this->ui->tabWidgetDatasets->count() - 1 && !this->isHidden()) {
        qDebug() << "Trigger upload new dataset";
    }
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


// ++++++++++++++++++++++++++++++++ MOUSE ++++++++++++++++++++++++++++++++
//void MainWindow::mousePressEvent(QMouseEvent * mousePressEvent) {
//    this->mouseClickXCoordinate = mousePressEvent->x();
//    this->mouseClickYCoordinate = mousePressEvent->y();
//}

//void MainWindow::mouseMoveEvent(QMouseEvent * mouseMoveEvent) {
//    this->move(mouseMoveEvent->globalX() - this->mouseClickXCoordinate, mouseMoveEvent->globalY() - this->mouseClickYCoordinate);
//}
