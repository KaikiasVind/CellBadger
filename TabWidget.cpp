#include <QTableWidgetItem>
#include <QDebug>

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

            this->ui->tableWidgetTypeCorrelations->setItem(i, j, tableWidgetItem);
        }
    }
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
    for (int i = 1; i < numberOfGeneIDs; i++) {
        geneIDHeaderItems.append(geneExpressions[0].getFeatureID(i));
    }

    // Add it to the table
    this->ui->tableWidgetGeneExpressions->setHorizontalHeaderLabels(clusterNameHeaderItems);
    this->ui->tableWidgetGeneExpressions->setVerticalHeaderLabels(geneIDHeaderItems);

    // Go through  every cluster and populate the table with the gene expression counts
    for (int i = 0; i < geneExpressions.length() - 1; i++) {
        for (int j = 0; j < geneExpressions[i].getNumberOfFeatures() - 1; j++) {
            double geneExpressionCount = geneExpressions[i].getFeatureExpressionCount(j);

            QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
            tableWidgetItem->setData(Qt::DisplayRole, geneExpressionCount);

            this->ui->tableWidgetGeneExpressions->setItem(i, j, tableWidgetItem);
        }
    }
}


/**
 * @brief TabWidget::on_lineEditGeneID_textEdited - When the line edit text has been edited the corresponding table is filtered according to the line edit content
 * @param lineEditContent - The string that is currently written in the line edit - Used to filter the table
 */
void TabWidget::on_lineEditGeneID_textEdited(const QString & lineEditContent) {
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

    // Filter list of gene IDs for search string and hide rows that don't contain it
    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount() - 1; i++) {
        if (!this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toLower().contains(searchString)) {
            this->ui->tableWidgetGeneExpressions->setRowHidden(i, true);
        }
    }

}
