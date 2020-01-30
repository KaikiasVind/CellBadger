#include <QTableWidgetItem>
#include <QDebug>
#include <QString>

#include "TabWidget.h"
#include "ui_TabWidget.h"
#include "BioModels/FeatureCollection.h"

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
void TabWidget::populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions) {
    int numberOfClusters = geneExpressions.length();
    int numberOfGeneIDs = geneExpressions[0].getNumberOfFeatures();

    this->ui->tableWidgetGeneExpressions->setColumnCount(numberOfClusters);
    this->ui->tableWidgetGeneExpressions->setRowCount(numberOfGeneIDs);

    // Create header with cluster numbers
    QStringList clusterNameHeaderItems;
    for (int i = 1; i < numberOfClusters + 1; i++) {
        clusterNameHeaderItems.append("Cluster " + QString::number(i));
    }

    // Create header with cluster numbers
    QStringList geneIDHeaderItems;
    for (int i = 0; i < numberOfGeneIDs; i++) {
        geneIDHeaderItems.append(geneExpressions[0].getFeatureID(i));
    }

    // Add it to the table
    this->ui->tableWidgetGeneExpressions->setHorizontalHeaderLabels(clusterNameHeaderItems);
    this->ui->tableWidgetGeneExpressions->setVerticalHeaderLabels(geneIDHeaderItems);

    // Go through  every cluster and populate the table with the gene expression counts
    for (int i = 0; i < geneExpressions.length(); i++) {
        for (int j = 0; j < geneExpressions[i].getNumberOfFeatures(); j++) {
            double geneExpressionCount = geneExpressions[i].getFeatureExpressionCount(j);

            QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
            tableWidgetItem->setData(Qt::DisplayRole, geneExpressionCount);

            this->ui->tableWidgetGeneExpressions->setItem(j, i, tableWidgetItem);
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
    QString searchString = lineEditContent.toUpper();

    // In case the user deleted the search string, just unhide the rows and return
    if (searchString == " ") {
        return;
    }

    QStringList searchStrings = searchString.split(",");

    // Filter list of gene IDs for search string and hide rows that don't contain it
    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
        bool isContainsAtLeastOneSearchString = false;
        for (QString string : searchStrings) {
            if (this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toUpper().contains(string)) {
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
            headerItemForSelectedRow = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(row)->text().toUpper(),
            newLineEditText;

    QStringList currentGeneIDs = currentLineEditText.split(",");

    for (int i = 0; i < currentGeneIDs.length(); i++) {
        QString geneID = currentGeneIDs[i].toUpper();

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
