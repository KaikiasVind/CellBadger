#include <QTableWidgetItem>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include "TabWidget.h"
#include "ui_TabWidget.h"
#include "BioModels/FeatureCollection.h"
#include "PlotWidget.h"

using QtCharts::QSplineSeries;
using QtCharts::QChart;
using QtCharts::QChartView;


TabWidget::TabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidget)
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


#include <iostream>

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

    // In case the user deleted the search string, just unhide the rows and return
    if (searchString == " ") {
        return;
    }

    QStringList searchStrings = searchString.split(",");

    // Filter list of gene IDs for search string and hide rows that don't contain it
    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
        bool isContainsAtLeastOneSearchString = false;
        for (QString string : searchStrings) {
            if (this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toLower().contains(string)) {
                isContainsAtLeastOneSearchString = true;
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
    QString currentLineEditText = this->ui->lineEditGeneID->text(),
            headerItemForSelectedRow = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(row)->text().toLower(),
            newLineEditText;

    QStringList currentGeneIDs = currentLineEditText.split(",");

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

    // Remove lasst space to avoid confusion with table - line edit comparisons
    if (currentLineEditText.endsWith(" ")) {
        newLineEditText.chop(1);
    }

    newLineEditText += headerItemForSelectedRow + ",";

    this->ui->lineEditGeneID->setText(newLineEditText);
}

void TabWidget::on_pushButtonPlot_clicked()
{
    QSplineSeries * series = new QSplineSeries();
    series->setName("spline");

    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);

    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Simple spline chart example");
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, 10);

    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    PlotWidget * plotWidget = new PlotWidget(chartView);
    plotWidget->resize(400, 300);
    plotWidget->show();
}
