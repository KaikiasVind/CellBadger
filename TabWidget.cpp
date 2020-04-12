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
    int numberOfClusters = geneExpressions.length();
    int numberOfGeneIDs = completeGeneIDs.length();

    // Resize the table according to the given number of clusters and gene IDs
    this->ui->tableWidgetGeneExpressions->setColumnCount(numberOfClusters);
    this->ui->tableWidgetGeneExpressions->setRowCount(numberOfGeneIDs);

    // Create header with cluster numbers
    QStringList clusterNameHeaderItems;
    for (int i = 1; i < numberOfClusters + 1; i++) {
        clusterNameHeaderItems.append("Cluster " + QString::number(i));
    }

    // Add the headers to the table
    this->ui->tableWidgetGeneExpressions->setHorizontalHeaderLabels(clusterNameHeaderItems);
    this->ui->tableWidgetGeneExpressions->setVerticalHeaderLabels(completeGeneIDs);

    // Go through the list of all gathered gene IDs
    // REMEMBER: UAGH 3 for loops
    for (int i = 0; i < numberOfGeneIDs; i++) {
        QString currentHeaderGeneID = completeGeneIDs[i];

        // And go through every cluster and check whether the gene is expressed or not
        for (int j = 0; j < numberOfClusters; j++) {
            bool isGeneExpressed = false;

            // Search for the current gene ID in the cluster -> Only contains IDs of expressed genes
            for (int k = 0; k < geneExpressions[j].getNumberOfFeatures(); k++) {
                QString currentClusterGeneID = geneExpressions[j].getFeature(k).ID;
                bool isSameString = currentHeaderGeneID.compare(currentClusterGeneID) == 0;

                // If the ID has been found (which means it is expressed in the cluster)
                if (isSameString) {
                    isGeneExpressed = true;
                    double geneExpressionCount = geneExpressions[j].getFeatureExpressionCount(k);

                    // Add a new TableWidgetItem to the corresponding cell containing the gene expression count
                    QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
                    tableWidgetItem->setData(Qt::DisplayRole, geneExpressionCount);

                    this->ui->tableWidgetGeneExpressions->setItem(i, j, tableWidgetItem);
                }
            }

            // If the gene is not found (which means its not expressed in the cluster), add a placeholder to the cell
            if (!isGeneExpressed) {
                QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
                tableWidgetItem->setData(Qt::DisplayRole, "< 1");

                this->ui->tableWidgetGeneExpressions->setItem(i, j, tableWidgetItem);
            }
        }
    }

    this->ui->tableWidgetGeneExpressions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


/**
 * @brief TabWidget::retrieveExpressionDataForSelectedGenes - Parse the gene expression table and grab the expression values for the selected genes
 * @return - List of QPairs: The QString is a geneID, the QVector is a list of the corresponding gene expressions for the clusters
 */
QVector<QPair<QString, QVector<double>>> TabWidget::retrieveExpressionDataForSelectedGenes() {
    QVector<QPair<QString, QVector<double>>> expressionDataForSelectedGenes;

    QStringList idsOfSelectedGenes = this->ui->lineEditGeneID->text().toLower().split(this->lineEditDelimiter);

    // Remove the " " after the last ,
    // REMEMBER: This should be done elsewhere
    idsOfSelectedGenes.removeLast();

    expressionDataForSelectedGenes.reserve(idsOfSelectedGenes.length());

    QVector<QScatterSeries *> allPlottingSeries;

    for (QString geneID : idsOfSelectedGenes) {
        QVector<double> expressionValues;
        expressionValues.reserve(8);

        bool isGeneHasBeenFound = false;

        qDebug() << geneID << ":";
        for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {

            // Compare the header item with the current gene ID
            bool isSameGeneID = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toLower().compare(geneID) == 0;

            // If the IDs are the same this means the correct row has been found, now collect information from the cells of the row
            if (isSameGeneID) {
                qDebug() << "Found";

                isGeneHasBeenFound = true;

                // Go through every cell and grab the expression value from that cell and append it to the current plotting series
                for (int j = 0; j < this->ui->tableWidgetGeneExpressions->columnCount(); j++) {

                    // Grab the expression value from the cell
                    // In case the conversion QString::toDouble does not work because the cell contained text, the value will be 0
                    double geneExpressionValueForCluster = this->ui->tableWidgetGeneExpressions->item(i, j)->text().toDouble();

                    qDebug() << "cluster" << j << "-" << geneExpressionValueForCluster;
                    expressionValues.append(geneExpressionValueForCluster);
                }
            }
        }

        // In case one of the gene IDs has not been found in the list, the gene ID is written wrong or is invalid otherwise
        if (!isGeneHasBeenFound) {
            qDebug() << "Not found.";
            this->showAlertForInvalidGeneID(geneID);
            return expressionDataForSelectedGenes;
        }

        expressionDataForSelectedGenes.append(qMakePair(geneID, expressionValues));
  }

    return expressionDataForSelectedGenes;
}

/**
 * @brief TabWidget::on_lineEditGeneID_textEdited - When the line edit text has been edited the corresponding table is filtered according to the line edit content
 * @param lineEditContent - The string that is currently written in the line edit - Used to filter the table
 */
void TabWidget::on_lineEditGeneID_textChanged(const QString & lineEditContent) {
    // Reset the previously hidden rows
    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
        this->ui->tableWidgetGeneExpressions->setRowHidden(i, false);
    }

    // Read search string from line edit
    QString searchString = lineEditContent.toLower();

    // In case the user deleted the search string, unhide the rows, disable the plot functionality and and return
    if (searchString == " " || searchString == "") {
        this->ui->pushButtonPlot->setEnabled(false);
        return;
    }

    QStringList searchStrings = searchString.split(this->lineEditDelimiter);

    // Filter list of gene IDs for search string and hide rows that don't contain it
    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
        bool isContainsAtLeastOneSearchString = false;
        for (QString string : searchStrings) {
            if (this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toLower().contains(string)) {
                isContainsAtLeastOneSearchString = true;

                // If a valid gene ID has been entered, enable the plotting functionality
                // REMEMBER: This does not work as intended, plotting should only been enabled when a valid ID has been entered
                if (!this->ui->pushButtonPlot->isEnabled())
                    this->ui->pushButtonPlot->setEnabled(true);
            }
        }
        if (!isContainsAtLeastOneSearchString) {
            this->ui->tableWidgetGeneExpressions->setRowHidden(i, true);
        }
    }

}

/**
 * @brief TabWidget::on_tableWidgetGeneExpressions_cellDoubleClicked - Adds the gene ID (header item) for the clicked item to the list of selected IDs - handles duplicates and autocomplete
 * @param row - Index of row that was clicked - used to get the corresponding header item (gene ID)
 * @param column - Unused
 */
void TabWidget::on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column) {

    Q_UNUSED(column);

    // If the first gene has been added, enable the plot functionality
    if (this->ui->lineEditGeneID->text() == "")
        this->ui->pushButtonPlot->setEnabled(true);

    QString currentLineEditText = this->ui->lineEditGeneID->text(),
            headerItemForSelectedRow = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(row)->text().toLower(),
            newLineEditText;

    QStringList currentGeneIDs = currentLineEditText.split(this->lineEditDelimiter);

    for (int i = 0; i < currentGeneIDs.length(); i++) {
        QString geneID = currentGeneIDs[i].toLower();

        // If the user clicks on an item that is already selected, return to prevent doubled items
        if (geneID == headerItemForSelectedRow) {
            qDebug() << "Duplicate item";
            return;
        }

        // If the user clicks on an item that he / she was beginning to type beforehand,
        // exchange the typed ID with the clicked ID to prevent doubled entries -> Autocomplete
        if (headerItemForSelectedRow.contains(geneID)) {
            qDebug() << "Found started item:" << geneID;
            currentGeneIDs.removeAt(i);
            currentGeneIDs.append(headerItemForSelectedRow);
            newLineEditText = currentGeneIDs.join(",").append(",");
            this->ui->lineEditGeneID->setText(newLineEditText);
            return;
        }
    }

    // If neither duplicates were found nor autocomplete could be done just add the item
    newLineEditText = currentLineEditText;

    // Remove last space to avoid confusion with table - line edit comparisons
    if (currentLineEditText.endsWith(" ")) {
        newLineEditText.chop(1);
    }

    newLineEditText += headerItemForSelectedRow + this->lineEditDelimiter;

    this->ui->lineEditGeneID->setText(newLineEditText);
}


/**
 * @brief TabWidget::on_pushButtonPlot_clicked
 */
void TabWidget::on_pushButtonPlot_clicked() {

    // If no genes have been selected, no plot can be generated, so return
    if (this->ui->lineEditGeneID->text() == "")
        return;

    QVector<QPair<QString, QVector<double>>> expressionDataForSelectedGenes = this->retrieveExpressionDataForSelectedGenes();

    // This case appears if at least one of the gene IDs is not found in the table and therefore is invalid
    if (expressionDataForSelectedGenes.isEmpty())
        return;

    QChart * chart = new QChart();

    // Collect the highest expression value for the correct y axis range
    double maxExpressionValue = .0;

    int numberOfClusters = expressionDataForSelectedGenes.first().second.length();

    for (QPair<QString, QVector<double>> selectedGene : expressionDataForSelectedGenes) {
        QScatterSeries * scatterSeries  = new QScatterSeries();
        scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        scatterSeries->setMarkerSize(10);

        double expressionSum = .0;

        for (int i = 0; i < selectedGene.second.length(); i++) {
            double expressionValue = selectedGene.second[i];

            scatterSeries->append(i, expressionValue);

            // If the current expression value is higher than the previously max value, store it as new max
            if (expressionValue >= maxExpressionValue)
                maxExpressionValue = expressionValue;

            expressionSum += expressionValue;
        }

        double geneMeanExpressionValue = (expressionSum / numberOfClusters);

        // Set the scatter series name with mean expression value
        scatterSeries->setName(selectedGene.first.toUpper() + " - mean: " + QString::number(geneMeanExpressionValue));

        // And add the scatter series for the current gene to the chart
        chart->addSeries(scatterSeries);
  }


    // Add basic configuration
    QString title = "Gene expression in " + this->title + " clusters";
    chart->setTitle(title);

    // Add the x axis with the cluster numbers
    QBarCategoryAxis * xAxis = new QBarCategoryAxis();
    xAxis->setTitleText(this->title + " clusters");

    // Add the cluster numbers to the x axis
    for (int i = 1; i < numberOfClusters + 1; i++) {
        xAxis->append(QString::number(i));
    }

    // Add the y axis with the relative UMI counts.
    // +1 for the range to make the marker for the max value more visible
    QValueAxis * yAxis = new QValueAxis();
    yAxis->setRange(0, maxExpressionValue + 1);

    yAxis->setTickCount(numberOfClusters);
    yAxis->setTitleText("relative UMI counts per cell");

    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Add the manually created axes to the chart view
    for (int i = 0; i < chart->series().length(); i++) {
        chartView->chart()->setAxisX(xAxis, chart->series().at(i));
        chartView->chart()->setAxisY(yAxis, chart->series().at(i));
    }

    ExportDialog * exportDialog = new ExportDialog(this);
    exportDialog->addPlot(chartView);
    exportDialog->show();
}


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
