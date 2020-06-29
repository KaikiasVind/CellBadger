#include <QTableWidgetItem>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QCategoryAxis>
#include <QLineSeries>

#include "TabWidget.h"
#include "ui_TabWidget.h"
#include "BioModels/FeatureCollection.h"
#include "ExportDialog.h"
#include "Utils/Plots.h"
#include "Utils/Models/GeneTableModel.h"

using QtCharts::QScatterSeries;
using QtCharts::QChart;
using QtCharts::QChartView;
using QtCharts::QValueAxis;
using QtCharts::QBarCategoryAxis;
using QtCharts::QCategoryAxis;
using QtCharts::QLineSeries;

TabWidget::TabWidget(QWidget *parent, QString title) :
    QWidget(parent),
    ui(new Ui::TabWidget),
    title(title)
{
    ui->setupUi(this);
}

TabWidget::~TabWidget()
{
    delete ui;
}

/**
 * @brief TabWidget::populateTableTypeCorrelations - Populates the table showing the top n correlations for each cluster.
 * @param correlations - List of clusters with corresponding type corrlations - sorted.
 * @param numberOfItems - Number of items that should be shown in the table.
 */
void TabWidget::populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, int numberOfItems) {
    int numberOfClusters = correlations.length();

    this->ui->tableWidgetTypeCorrelations->setColumnCount(numberOfClusters);
    this->ui->tableWidgetTypeCorrelations->setRowCount(numberOfItems);

    // Create header with cluster numbers
    QStringList clusterNameHeaderItems;
    for (int i = 1; i < numberOfClusters + 1; i++) {
        clusterNameHeaderItems.append("Cluster " + QString::number(i));
    }

    // Add it to the table
    this->ui->tableWidgetTypeCorrelations->setHorizontalHeaderLabels(clusterNameHeaderItems);

    // Go through the top n of every cluster and populate the table with it
    for (int i = 0; i < correlations.length(); i++) {
        for (int j = 0; j < numberOfClusters; j++) {
            QPair<QString, double> type = correlations[i][j];
            QString cell = QString::number(type.second) + ": " + type.first;

            // A TableWidgetItem is needed for every cell
            QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
            tableWidgetItem->setData(Qt::DisplayRole, cell);

            this->ui->tableWidgetTypeCorrelations->setItem(j, i, tableWidgetItem);
        }
    }

    this->ui->tableWidgetTypeCorrelations->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


/**
 * @brief TabWidget::populateTableGeneExpressions - Populates the gene expression table with the gene expression counts
 * * @param geneExpressions - list of clusters with corresponding gene expression counts - unsorted.
 */
void TabWidget::populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs) {

//    if (this->geneTableModel != nullptr)
//        delete this->geneTableModel;

    this->geneTableModel = new GeneTableModel(geneExpressions, completeGeneIDs);
    this->proxyModel = new ProxyModel(completeGeneIDs.length() + 1, geneExpressions.length() + 1);
    this->proxyModel->setSourceModel(geneTableModel);

    this->tableView = new QTableView;
    this->tableView->setModel(this->proxyModel);

    this->tableView->verticalHeader()->setMinimumWidth(25);
    this->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    this->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->tableView->setSortingEnabled(true);
    this->tableView->sortByColumn(0, Qt::AscendingOrder);
    this->ui->horizontalLayoutGeneExpressionTable->insertWidget(0, this->tableView);

//    int numberOfClusters = geneExpressions.length();
//    int numberOfGeneIDs = completeGeneIDs.length();

//    // Resize the table according to the given number of clusters and gene IDs
//    // +1 for column count because of the mean value column
//    this->ui->tableWidgetGeneExpressions->setColumnCount(numberOfClusters + 1);
//    this->ui->tableWidgetGeneExpressions->setRowCount(numberOfGeneIDs);

//    // Create header with cluster numbers
//    QStringList clusterNameHeaderItems;
//    for (int i = 1; i < numberOfClusters + 1; i++) {
//        clusterNameHeaderItems.append("Cluster " + QString::number(i));
//    }

//    // Add one column that will hold the mean of the expression values for all clusters
//    clusterNameHeaderItems.append("⌀");

//    // Add the headers to the table
//    this->ui->tableWidgetGeneExpressions->setHorizontalHeaderLabels(clusterNameHeaderItems);
//    this->ui->tableWidgetGeneExpressions->setVerticalHeaderLabels(completeGeneIDs);

//    // Go through the list of all gathered gene IDs
//    // REMEMBER: UAGH 3 for loops
//    for (int i = 0; i < numberOfGeneIDs; i++) {
//        QString currentHeaderGeneID = completeGeneIDs[i];

//        double expressionSum = 0;

//        // And go through every cluster and check whether the gene is expressed or not
//        for (int j = 0; j < numberOfClusters; j++) {
//            bool isGeneExpressed = false;

//            // Search for the current gene ID in the cluster -> Only contains IDs of expressed genes
//            for (int k = 0; k < geneExpressions[j].getNumberOfFeatures(); k++) {
//                QString currentClusterGeneID = geneExpressions[j].getFeature(k).ID;
//                bool isSameString = currentHeaderGeneID.compare(currentClusterGeneID) == 0;

//                // If the ID has been found (which means it is expressed in the cluster)
//                if (isSameString) {
//                    isGeneExpressed = true;
//                    double geneExpressionCount = geneExpressions[j].getFeatureExpressionCount(k);

//                    // Add a new TableWidgetItem to the corresponding cell containing the gene expression count
//                    QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
//                    tableWidgetItem->setData(Qt::DisplayRole, geneExpressionCount);

//                    this->ui->tableWidgetGeneExpressions->setItem(i, j, tableWidgetItem);

//                    expressionSum += geneExpressionCount;
//                }
//            }

//            // If the gene is not found (which means its not expressed in the cluster), add a placeholder to the cell
//            if (!isGeneExpressed) {
//                QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
//                tableWidgetItem->setData(Qt::DisplayRole, "< 1");

//                this->ui->tableWidgetGeneExpressions->setItem(i, j, tableWidgetItem);
//            }

//            // Compute the mean of the current gene's expression values in all clusters and add it to the table
//            double meanExpressionCount = (expressionSum / numberOfClusters);

//            // Set the mean expression count as item into the last table column
//            QTableWidgetItem * tableWidgetItemMeanCount = new QTableWidgetItem(0);
//            tableWidgetItemMeanCount->setData(Qt::DisplayRole, "⌀: " + QString::number(meanExpressionCount));
//            this->ui->tableWidgetGeneExpressions->setItem(i, numberOfClusters, tableWidgetItemMeanCount);
//        }
//    }

//    this->ui->tableWidgetGeneExpressions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


/**
 * @brief TabWidget::retrieveExpressionDataForSelectedGenes - Parse the gene expression table and grab the expression values for the selected genes
 * @return - List of QPairs: The QString is a geneID, the QVector is a list of the corresponding gene expressions for the clusters
 */
//QVector<QPair<QString, QPair<QVector<double>, double>>> TabWidget::retrieveExpressionDataForSelectedGenes() {
//    QVector<QPair<QString, QPair<QVector<double>, double>>> expressionDataForSelectedGenes;

//    QStringList idsOfSelectedGenes = this->ui->lineEditGeneID->text().toLower().split(this->lineEditDelimiter);

//    // Remove the " " after the last ,
//    // REMEMBER: This should be done elsewhere
//    idsOfSelectedGenes.removeLast();

//    expressionDataForSelectedGenes.reserve(idsOfSelectedGenes.length());

//    // For every selected gene go through every cluster and grab the expression values
//    for (QString geneID : idsOfSelectedGenes) {
//        QVector<double> expressionValues;
//        // Reserve space in the list for the n expression values of the n clusters
//        // -1 because the last column is reserved for the mean value
//        expressionValues.reserve(this->ui->tableWidgetGeneExpressions->rowCount() - 1);

//        double meanGeneExpressionValue = 0.;

//        bool isGeneHasBeenFound = false;

//        for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {

//            // Compare the header item with the current gene ID
//            bool isSameGeneID = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toLower().compare(geneID) == 0;

//            // If the IDs are the same this means the correct row has been found, now collect information from the cells of the row
//            if (isSameGeneID) {
//                isGeneHasBeenFound = true;

//                // Go through every cell and grab the expression value from that cell and append it to the current plotting series
//                for (int j = 0; j < this->ui->tableWidgetGeneExpressions->columnCount(); j++) {

//                    // Grab the expression value from the cell
//                    // In case the conversion QString::toDouble does not work because the cell contained text, the value will be 0
//                    double geneExpressionValueForCluster = this->ui->tableWidgetGeneExpressions->item(i, j)->text().toDouble();

//                    // The last column is reserved for the mean value, if this column is reached, save the current gene's mean value
//                    if (j == this->ui->tableWidgetGeneExpressions->columnCount() - 1) {
//                        // REMEMBER: Exchange with RegEx?
//                        meanGeneExpressionValue = this->ui->tableWidgetGeneExpressions->item(i, j)->text().split(" ").last().toDouble();
//                    } else {
//                        expressionValues.append(geneExpressionValueForCluster);
//                    }
//                }
//            }
//        }

//        // In case one of the gene IDs has not been found in the list, the gene ID is written wrong or is invalid otherwise
//        if (!isGeneHasBeenFound) {
//            this->showAlertForInvalidGeneID(geneID);
//            return expressionDataForSelectedGenes;
//        }

//        expressionDataForSelectedGenes.append(qMakePair(geneID, qMakePair(expressionValues, meanGeneExpressionValue)));
//  }

//    return expressionDataForSelectedGenes;
//}

/**
 * @brief TabWidget::on_lineEditGeneID_textEdited - When the line edit text has been edited the corresponding table is filtered according to the line edit content
 * @param lineEditContent - The string that is currently written in the line edit - Used to filter the table
 */
//void TabWidget::on_lineEditGeneID_textChang/*ed(const QString & lineEditContent) {
    // Reset the previously hidden rows
//    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
//        this->ui->tableWidgetGeneExpressions->setRowHidden(i, false);
//    }

//    // Read search string from line edit
//    QString searchString = lineEditContent.toLower();

//    // In case the user deleted the search string, unhide the rows, disable the plot functionality and and return
//    if (searchString == " " || searchString == "") {
//        this->ui->pushButtonScatterPlot->setEnabled(false);
//        this->ui->pushButtonScatterPlot->setEnabled(false);
//        return;
//    }

//    QStringList searchStrings = searchString.split(this->lineEditDelimiter);

//    // Filter list of gene IDs for search string and hide rows that don't contain it
//    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
//        bool isContainsAtLeastOneSearchString = false;
//        for (QString string : searchStrings) {
//            if (this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toLower().contains(string)) {
//                isContainsAtLeastOneSearchString = true;

//                // If a valid gene ID has been entered, enable the plotting functionality
//                // REMEMBER: This does not work as intended, plotting should only been enabled when a valid ID has been entered
//                if (!this->ui->pushButtonScatterPlot->isEnabled() && !this->ui->pushButtonBarChart->isEnabled()) {
//                    this->ui->pushButtonScatterPlot->setEnabled(true);
//                    this->ui->pushButtonBarChart->setEnabled(true);
//                }
//            }
//        }
//        if (!isContainsAtLeastOneSearchString) {
//            this->ui->tableWidgetGeneExpressions->setRowHidden(i, true);
//        }
//    }

//}

/**
 * @brief TabWidget::on_tableWidgetGeneExpressions_cellDoubleClicked - Adds the gene ID (header item) for the clicked item to the list of selected IDs - handles duplicates and autocomplete
 * @param row - Index of row that was clicked - used to get the corresponding header item (gene ID)
 * @param column - Unused
 */
//void TabWidget::on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column) {

//    Q_UNUSED(column);

//    // If the first gene has been added, enable the plot functionality
//    if (this->ui->lineEditGeneID->text() == "") {
//        this->ui->pushButtonScatterPlot->setEnabled(true);
//        this->ui->pushButtonBarChart->setEnabled(true);
//    }

//    QString currentLineEditText = this->ui->lineEditGeneID->text(),
//            headerItemForSelectedRow = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(row)->text().toLower(),
//            newLineEditText;

//    QStringList currentGeneIDs = currentLineEditText.split(this->lineEditDelimiter);

//    for (int i = 0; i < currentGeneIDs.length(); i++) {
//        QString geneID = currentGeneIDs[i].toLower();

//        // If the user clicks on an item that is already selected, return to prevent doubled items
//        if (geneID == headerItemForSelectedRow) {
//            qDebug() << "Duplicate item";
//            return;
//        }

//        // If the user clicks on an item that he / she was beginning to type beforehand,
//        // exchange the typed ID with the clicked ID to prevent doubled entries -> Autocomplete
//        if (headerItemForSelectedRow.contains(geneID)) {
//            qDebug() << "Found started item:" << geneID;
//            currentGeneIDs.removeAt(i);
//            currentGeneIDs.append(headerItemForSelectedRow);
//            newLineEditText = currentGeneIDs.join(",").append(",");
//            this->ui->lineEditGeneID->setText(newLineEditText);
//            return;
//        }
//    }

//    // If neither duplicates were found nor autocomplete could be done just add the item
//    newLineEditText = currentLineEditText;

//    // Remove last space to avoid confusion with table - line edit comparisons
//    if (currentLineEditText.endsWith(" ")) {
//        newLineEditText.chop(1);
//    }

//    newLineEditText += headerItemForSelectedRow + this->lineEditDelimiter;

//    this->ui->lineEditGeneID->setText(newLineEditText);
//}


/**
 * @brief TabWidget::showAlertForInvalidGeneID - Show an alert with the given gene ID
 * @param geneID - ID that is shown as invalid in the alert
 */
void TabWidget::showAlertForInvalidGeneID(QString geneID) {
    QMessageBox invalidGeneIDAlert;
    invalidGeneIDAlert.setText("Invalid gene ID: " + geneID);
    invalidGeneIDAlert.setWindowFlags(Qt::FramelessWindowHint);
    invalidGeneIDAlert.exec();
    return;
}


//template<typename F>
/**
 * @brief TabWidget::openExportWidgetWithPlot - Ceates a plot with the given plotting function and opens it in an ExportDialog
 * @param plottingFunction - Function that creates a QChartView * that is used to create a plot which is then transfered onto an ExportDialog
 */
//void TabWidget::openExportWidgetWithPlot(F plottingFunction) {

//    // If no genes have been selected, no plot can be generated, so return
//    if (this->ui->lineEditGeneID->text() == "")
//        return;

//    QVector<QPair<QString, QPair<QVector<double>, double>>> expressionDataForSelectedGenes = this->retrieveExpressionDataForSelectedGenes();

//    // This case appears if at least one of the gene IDs is not found in the table and therefore is invalid
//    if (expressionDataForSelectedGenes.isEmpty())
//        return;

//    QChartView * chartView = plottingFunction(expressionDataForSelectedGenes, this->title);

//    ExportDialog * exportDialog = new ExportDialog(this);
//    exportDialog->addPlot(chartView);
//    exportDialog->show();
//}


/**
 * @brief TabWidget::on_pushButtonPlot_clicked
 */
//void TabWidget::on_pushButtonScatterPlot_clicked() {
//    this->openExportWidgetWithPlot(Plots::createScatterPlot);
//}


/**
 * @brief TabWidget::on_pushButtonBarChart_clicked
 */
//void TabWidget::on_pushButtonBarChart_clicked() {
//    this->openExportWidgetWithPlot(Plots::createBarChart);
//}

/**
 * @brief TabWidget::on_pushButtonCorrelationOptionsRun_clicked
 */
//void TabWidget::on_pushButtonCorrelationOptionsRun_clicked() {

//}
