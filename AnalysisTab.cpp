#include "AnalysisTab.h"
#include "ui_AnalysisTab.h"

#include <QtCharts/QChartView>
#include <QDebug>

#include "Utils/Plots.h"
#include "Utils/Helper.h"
#include "PlotButton.h"
#include "ExportDialog.h"
#include "Utils/FileOperators/CSVReader.h"

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

    int columnIndex = this->ui->tableWidgetExperimentsSelection->columnCount();
    int numberOfNecessaryRows = experiment.length() + 1;

    // Insert an empty column for the new values
    this->ui->tableWidgetExperimentsSelection->insertColumn(columnIndex);

    // And add additional rows in case the experiment has more clusters than previously added experiments
    if (this->ui->tableWidgetExperimentsSelection->rowCount() < numberOfNecessaryRows)
        this->ui->tableWidgetExperimentsSelection->setRowCount(numberOfNecessaryRows);

    // Create a new header item for the new experiment and add it to the horizontal header
    QTableWidgetItem * newHeaderItem = new QTableWidgetItem(0);
    newHeaderItem->setData(Qt::DisplayRole, experimentName);
    newHeaderItem->setTextAlignment(Qt::AlignCenter);
    this->ui->tableWidgetExperimentsSelection->setHorizontalHeaderItem(columnIndex, newHeaderItem);

    // Add new WidgetItem to the first row that will hold a plot button
    this->addPlotButtonItemToTable(columnIndex);

    // Add every cluster to the table
    for (int rowIndex = 1; rowIndex < numberOfNecessaryRows; rowIndex++) {
        // Because an additional row field was created for the plot button,
        // the row index is one number higher than the row index, thus subtract 1
        int contentIndex = rowIndex - 1;

        QTableWidgetItem * newTableItem = new QTableWidgetItem(0);
        QString tableItemName = experiment.at(contentIndex).ID + ": ";

        // Add the cell type to the cluster name that is most closely correlated to it
        // If no correlation was found for the current cluster, just append NA to the name
        if (correlations.at(contentIndex).at(0).second == 0) {
            tableItemName.append("NA");
        } else {
            tableItemName.append(correlations.at(contentIndex).at(0).first);
        }

        newTableItem->setData(Qt::DisplayRole, tableItemName);
        newTableItem->setTextAlignment(Qt::AlignLeft);
        this->ui->tableWidgetExperimentsSelection->setItem(rowIndex, columnIndex, newTableItem);
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
 * @brief AnalysisTab::addPlotButtonItemToTable - Add a new table item with a plot button to the table at the given index
 * @param columnIndex - Column index where the new button is to be added
 */
void AnalysisTab::addPlotButtonItemToTable(const int columnIndex) {
    QTableWidgetItem * buttonItem = new QTableWidgetItem(0);
    this->ui->tableWidgetExperimentsSelection->setItem(0, columnIndex, buttonItem);

    // Create a plot button
    QWidget * buttonWidget = new QWidget();
    PlotButton * button = new PlotButton("t-SNE plot", this, columnIndex);

    QObject::connect(button, &PlotButton::clickedWithIndex, this, &AnalysisTab::on_plotButtonClicked);

    QHBoxLayout * layout = new QHBoxLayout(buttonWidget);
    layout->addWidget(button);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    buttonWidget->setLayout(layout);

    // Add the plot button to the previously created WidgetItem
    this->ui->tableWidgetExperimentsSelection->setCellWidget(0, columnIndex, buttonWidget);

    // Make the newly created field non-selectable, so it will not be selected by accident
    this->ui->tableWidgetExperimentsSelection->item(0, columnIndex)->setFlags(Qt::NoItemFlags);
}

// ################################################## SLOTS ###########################################################

/**
 * @brief AnalysisTab::on_receivedGeneExpressionData
 * @param experiments
 */
void AnalysisTab::on_receivedGeneExpressionData(const QVector<QVector<FeatureCollection>> experiments, const QVector<QStringList> completeSetsOfGeneIDsPerDataset) {
    // Gather all gene-IDs the user put into the text edit and filter out empty IDs
    QStringList foundGeneIDs = this->ui->textEditGeneSelection->toPlainText().split(",");
    foundGeneIDs.removeAll(QString(""));

    // If no gene IDs have been selected, return
    if (foundGeneIDs.isEmpty())
        return;

    // Flatten the list of complete gene IDs into a set for easier handling
    QSet<QString> completeGeneIDs;
    for (QStringList list : completeSetsOfGeneIDsPerDataset) {
        for (QString geneID : list)
            completeGeneIDs.insert(geneID);
    }

    QModelIndexList selectedIndices = this->ui->tableWidgetExperimentsSelection->selectionModel()->selectedIndexes();

    QMap<QString, QVector<double>> geneExpressionValuesForSelectedClusters;
    QStringList clusterNames;

    for (QModelIndex selectedIndex : selectedIndices) {
        // Because an additional row field was created for the plot button,
        // the row index is one number higher than the row index, thus subtract 1
        int contentRowIndex = selectedIndex.row() - 1;

        int column = selectedIndex.column();

        // The selected experiment corresponds to the column that was selected in the table
        QVector<FeatureCollection> selectedExperiment = experiments.at(column);

        // If a cluster is selected that doesn't exist for the selected experiment. skip it.
        // This happens if the experiments have a different amount of clusters
        if (contentRowIndex > selectedExperiment.size() - 1)
            continue;

        // Append either the type of the selected cell or the cluster name if there is no type
        QString clusterName = "Experiment " + QString::number(selectedIndex.column()) + " : ";
        QStringList type = selectedIndex.data(Qt::DisplayRole).toString().split(": ");
        clusterName.append(type[1].compare("NA") == 0 ? type[0] : type[1]);
        clusterNames.append(clusterName);

        // The selected cluster corresponds to the row that was selected in the table
        FeatureCollection selectedCluster = selectedExperiment.at(contentRowIndex);

        // Look for every gene that was written in the text input and in every selected cluster
        for (QString geneID : foundGeneIDs) {
            geneID = geneID.toUpper();

            // If the entered geneID doesn't exist in any experiment, skip it
            if (!completeGeneIDs.contains(geneID))
                continue;

            // Else search for the gene in the selected cluster
            Feature foundFeature = selectedCluster.getFeature(geneID);

            // If the gene is not expressed in the cluster, add 0 as expression value to the list
            if (foundFeature.ID.compare("nAn") == 0)
                geneExpressionValuesForSelectedClusters[geneID].append(0);
            // Else add the gene expression value of the gene found in the cluster
            else
                geneExpressionValuesForSelectedClusters[geneID].append(foundFeature.count);
        }
    }

    // If no cluster has been selected, return
    // This happens if ONLY a cluster has been selected that doesn't exist in the experiment
    if (clusterNames.isEmpty())
        return;

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

void AnalysisTab::on_plotButtonClicked(const int buttonIndex) {
    ExportDialog * tSNEPlotDialog = new ExportDialog();
    QString tSNEFilePath = Helper::openLoadFileDialog(this, {"csv"}, false).first();

    QVector<std::tuple<QString, int, double, double>> tsneProjectionData = CSVReader::readTSNECoordinatesFromProjectionFile(tSNEFilePath);

    std::sort(tsneProjectionData.begin(), tsneProjectionData.end(), []
                   (const std::tuple<QString, int, double, double> valueA, const std::tuple<QString, int, double, double> valueB) {
        return std::get<1>(valueA) < std::get<1>(valueB);
    });

    QStringList clusterTypes;

    for (int i = 0; i < this->ui->tableWidgetExperimentsSelection->rowCount(); i++) {
        QString itemName = this->ui->tableWidgetExperimentsSelection->item(i, buttonIndex)->text();
        clusterTypes.append(itemName);
    }

    // CREATE NEW SCATTERSERIES SUBCLASS WITH A CLICKED SIGNAL THAT CAN TRANSFER THE INDEX OF THE CLICKED SERIES
    // Look into int index = your_containter.indexOf(your_serie);
    // Taken from https://stackoverflow.com/questions/51744802/qchartview-qlineseries-select-by-mouse-click
    QChartView * chartView = Plots::createUMAPPlot("UMAP projection of clusters", tsneProjectionData, this, clusterTypes);
//    chartView->chart()->series().inde

    tSNEPlotDialog->addPlot(chartView);
    tSNEPlotDialog->show();
    qDebug() << "Click click on:" << buttonIndex;
}

/**
 * @brief ExportDialog::on_lineSeriesClicked
 */
void AnalysisTab::on_lineSeriesClickedWithIndex(const int index, ScatterSeries const *series) {
    qDebug() << "Clicked on line series" << index;
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
