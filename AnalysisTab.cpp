#include "AnalysisTab.h"
#include "ui_AnalysisTab.h"

#include <QtCharts/QChartView>

#include "Utils/Plots.h"
#include "Utils/Helper.h"

using QtCharts::QChartView;


AnalysisTab::AnalysisTab(QWidget *parent) :
    QWidget(parent), ui(new Ui::AnalysisTab)
{

    ui->setupUi(this);
    this->ui->tableWidgetExperimentsSelection->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

AnalysisTab::~AnalysisTab() {
    delete ui;
}


/**
 * @brief AnalysisTab::addExperiment - Add cluster-names and highest correlated cell type to a comparison list of all experiments
 * @param experimentName - Name of the experiment
 * @param experiment - Gene expression data of the experiment
 * @param correlations - List of cell types sorted by highest correlation to the clusters of the given experiment
 */
void AnalysisTab::addExperiment(const QString experimentName, const QVector<FeatureCollection> experiment, const QVector<QVector<QPair<QString, double>>> correlations) {

    // Insert an empty column for the new values
    this->ui->tableWidgetExperimentsSelection->insertColumn(0);

    // And add additional rows in case the experiment has more clusters than previously added experiments
    if (this->ui->tableWidgetExperimentsSelection->rowCount() < experiment.length())
        this->ui->tableWidgetExperimentsSelection->setRowCount(experiment.length());


    // Create a new header item for the new experiment and add it to the horizontal header
    QTableWidgetItem * newHeaderItem = new QTableWidgetItem(0);
    newHeaderItem->setData(Qt::DisplayRole, experimentName);
    newHeaderItem->setTextAlignment(Qt::AlignCenter);
    this->ui->tableWidgetExperimentsSelection->setHorizontalHeaderItem(0, newHeaderItem);

    // Add every cluster to the table
    for (int i = 0; i < experiment.length(); i++) {
        QTableWidgetItem * newTableItem = new QTableWidgetItem(0);
        QString tableItemName = experiment.at(i).ID + ": ";

        // Add the cell type to the cluster name that is most closely correlated to it
        // If no correlation was found for the current cluster, just append NA to the name
        if (correlations.at(i).at(0).second == 0)
            tableItemName.append("NA");
        else
            tableItemName.append(correlations.at(i).at(0).first);

        newTableItem->setData(Qt::DisplayRole, tableItemName);
        newTableItem->setTextAlignment(Qt::AlignLeft);
        this->ui->tableWidgetExperimentsSelection->setItem(i, 0, newTableItem);
    }
}


/**
 * @brief AnalysisTab::cleanTable - Delete every QTableWidgetItem in the table to make room for more
 */
void AnalysisTab::cleanTable() {
    // Delete all QTableWidgets that have been initialised with the new operator
    for (int i = 0; i < this->ui->tableWidgetExperimentsSelection->rowCount(); i++) {
        for (int j = 0; j < this->ui->tableWidgetExperimentsSelection->columnCount(); j++) {
            delete this->ui->tableWidgetExperimentsSelection->item(i, j);
        }
    }

    // Set the column count to zero, this removes all visible columns
    this->ui->tableWidgetExperimentsSelection->setColumnCount(0);
}


/**
 * @brief AnalysisTab::on_receivedGeneExpressionData
 * @param experiments
 */
void AnalysisTab::on_receivedGeneExpressionData(const QVector<QVector<FeatureCollection>> experiments) {
    // Gather all gene-IDs the user put into the text edit and filter out empty IDs
    QStringList foundGeneIDs = this->ui->textEditGeneSelection->toPlainText().split(",");
    foundGeneIDs.removeAll(QString(""));

    // If no gene IDs have been selected, return
    if (foundGeneIDs.isEmpty())
        return;

    QModelIndexList selectedIndices = this->ui->tableWidgetExperimentsSelection->selectionModel()->selectedIndexes();

    QMap<QString, QVector<double>> geneExpressionValuesForSelectedClusters;
    QStringList clusterNames;

    for (QModelIndex selectedIndex : selectedIndices) {

        // Append the text of the selected cell to the list of names of the selected clusters
        clusterNames.append(selectedIndex.data(Qt::DisplayRole).toString());

        // REMEMBER: FIXME!!
        // The experiments are displayed in the reversed order compared to how they are stored
        // To cope with this, the columns have to be converted to the correct cluster
        int column = abs(selectedIndex.column() - (experiments.size() - 1));

        // The selected experiment corresponds to the column that was selected in the table
        QVector<FeatureCollection> selectedExperiment = experiments.at(column);

        // The selected cluster corresponds to the row that was selected in the table
        FeatureCollection selectedCluster = selectedExperiment.at(selectedIndex.row());

        // Look for every gene that was written in the text input and in every selected cluster
        for (QString geneID : foundGeneIDs) {
            geneID = geneID.toUpper();
            Feature foundFeature = selectedCluster.getFeature(geneID);

            // If the gene is not expressed in the cluster, add 0 as expression value to the list
            if (foundFeature.ID.compare("nAn") == 0)
                geneExpressionValuesForSelectedClusters[geneID].append(0);
            // Else add the gene expression value of the gene found in the cluster
            else
                geneExpressionValuesForSelectedClusters[geneID].append(foundFeature.count);
        }
    }

    QString plotTitle = "selected";
    QString yAxisTitle = "Raw counts per cluster / relative UMI counts per cell";

    QChartView * chart;
    switch (this->requestedPlotType) {
        case PlotType::SCATTER_PLOT:
        chart = Plots::createScatterPlot(plotTitle, yAxisTitle, geneExpressionValuesForSelectedClusters, clusterNames);
            break;
    }

    Helper::openExportWidgetWithPlot(chart);
}

// ########################## UI-SLOTS ##########################
void AnalysisTab::on_pushButtonScatterPlot_clicked() {
    this->requestedPlotType = PlotType::SCATTER_PLOT;
    emit this->requestGeneExpressionData();
}

void AnalysisTab::on_pushButtonBarChart_clicked() {
    return;
    emit this->requestGeneExpressionData();
}
