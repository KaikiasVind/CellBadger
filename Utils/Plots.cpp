#include "Plots.h"

#include <QVector>
#include <QPair>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QMap>

#include <QDebug>

#include "BioModels/FeatureCollection.h"
#include "ExportDialog.h"

using QtCharts::QScatterSeries;
using QtCharts::QChart;
using QtCharts::QChartView;
using QtCharts::QValueAxis;
using QtCharts::QBarCategoryAxis;
using QtCharts::QCategoryAxis;
using QtCharts::QLineSeries;
using QtCharts::QBarSet;
using QtCharts::QBarSeries;
using QtCharts::QLegend;

namespace Plots {


/**
 * @brief createScatterPlot - Creates a scatter plot for the given gene expression data - The resulting plot contains one plotting series per given gene
 * @param title - The title the resulting plot should show
 * @param geneExpressionValuesForClusters - Map of genes with corresponding expression data for n clusters
 * @param clusterNames - List of names of the clusters the genes are expressed in
 * @param meanValues - List of means of the gene expressions in the given clusters
 * @return - A plot that combines one plotting series per given gene in one scatter plot
 */
QChartView * createScatterPlot(const QString title, const QString yAxisTitle, const QMap<QString, QVector<double>> expressionDataForGenesInClusters, const QStringList clusterNames, const QVector<double> meanValues) {
    QChart * chart = new QChart();

    // Collect the highest expression value for the correct y axis range
    double maxExpressionValue = 0.;

    for (int i = 0; i < clusterNames.length(); i++) {
        QScatterSeries * scatterSeries = new QScatterSeries();
        scatterSeries->setName(clusterNames.at(i));
        scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        scatterSeries->setMarkerSize(8);

        for (int j = 0; j < expressionDataForGenesInClusters.keys().length(); j++) {
            QString geneID = expressionDataForGenesInClusters.keys().at(j);
            double expressionValue = expressionDataForGenesInClusters[geneID].at(i);

            // If the current expression value is higher than the previously max value, store it as new max
            if (expressionValue >= maxExpressionValue)
                maxExpressionValue = expressionValue;

            // Add the value to the scatter series
            scatterSeries->append(j, expressionValue);
        }

        chart->addSeries(scatterSeries);
    }

    // If mean values have been given, add them as additional series
    if (!meanValues.isEmpty()) {
        // Create a series for mean values
        QScatterSeries * meanScatterSeries = new QScatterSeries();
        meanScatterSeries->setName("mean");
        meanScatterSeries->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        meanScatterSeries->setMarkerSize(8);

        for (int i = 0; i < meanValues.length(); i++)
        meanScatterSeries->append(i, meanValues.at(i));

        chart->addSeries(meanScatterSeries);
    }

    // Add basic configuration
    QString plotTitle = "Gene expression in " + title + " clusters";
    chart->setTitle(plotTitle);

    // Add the x axis with the cluster numbers
    QBarCategoryAxis * xAxis = new QBarCategoryAxis();
    xAxis->setTitleText(title + " genes");
    xAxis->append(expressionDataForGenesInClusters.keys());

    // Add the y axis with the relative UMI counts.
    // +1 for the range to make the marker for the max value more visible
    QValueAxis * yAxis = new QValueAxis();
    yAxis->setRange(0, maxExpressionValue + 5);

    yAxis->setTickCount(clusterNames.length());
    yAxis->setTitleText(yAxisTitle);

    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Add the manually created axes to the chart view
    for (int i = 0; i < chart->series().length(); i++) {
        chartView->chart()->setAxisX(xAxis, chart->series().at(i));
        chartView->chart()->setAxisY(yAxis, chart->series().at(i));
    }

    return chartView;
}

/**
 * @brief createUMAPPlot
 * @param title
 * @param tSNEProjectionData - SORTED BY CLUSTER INDEX!!!
 * @return
 */
QChartView * createUMAPPlot(const QString title, const QVector<std::tuple<QString, int, double, double>> tSNEProjectionData, AnalysisTab * analysisTab) {
    QChart * chart = new QChart();
    QVector<QScatterSeries *> allScatterSeries;

    double minObservedXValue = std::get<2>(tSNEProjectionData.first()),
           maxObservedXValue = std::get<2>(tSNEProjectionData.first()),
           minObservedYValue = std::get<3>(tSNEProjectionData.first()),
           maxObservedYValue = std::get<3>(tSNEProjectionData.first());

    QVector<int> clusterIndices;

    for (auto const & value : tSNEProjectionData) {
        int newClusterIndex = std::get<1>(value);

        // If this is the first round or there is a new index,
        // append a new scatterseries and save the cluster's index
        if (clusterIndices.isEmpty() || newClusterIndex != clusterIndices.last()) {
            clusterIndices.append(newClusterIndex);
            allScatterSeries.append(new QScatterSeries());
        }

        double newXValue = std::get<2>(value),
               newYValue = std::get<3>(value);

        if (newXValue < minObservedXValue)
            minObservedXValue = newXValue;
        else if (newXValue > maxObservedXValue)
            maxObservedXValue = newXValue;

        if (newYValue < minObservedYValue)
            minObservedYValue = newYValue;
        else if (newYValue > maxObservedYValue)
            maxObservedYValue = newYValue;

        allScatterSeries.last()->append(std::get<2>(value), std::get<3>(value));
    }

    for (int i = 0; i < allScatterSeries.length(); i++) {
        QScatterSeries * series = allScatterSeries.at(i);
        chart->addSeries(series);
        series->setName("Cluster " + QString::number(clusterIndices.at(i)));
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(8);
        QObject::connect(series, &QScatterSeries::clicked, analysisTab, &AnalysisTab::on_lineSeriesClicked);
    }

    chart->setTitle("Chart test");
    chart->setDropShadowEnabled(false);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QValueAxis * xAxis = new QValueAxis();
    QValueAxis * yAxis = new QValueAxis();
    xAxis->setTitleText("UMAP 1");
    yAxis->setTitleText("UMAP 2");
    xAxis->setRange(minObservedXValue - 1, maxObservedXValue + 1);
    yAxis->setRange(minObservedYValue - 1, maxObservedYValue + 1);

    // Add axes to the chart
    for (auto const & series : chart->series()) {
        chart->setAxisX(xAxis, series);
        chart->setAxisY(yAxis, series);
    }

    return chartView;
}

///**
// * @brief createBarChart - Creates a bar chart for the given gene expression data - The resulting chart contains one barset per given gene
// * @param geneExpressionValuesForClusters - List of genes with corresponding expression data for n clusters
// * @param plotTitle - The title the resulting plot should show
// * @return - A bar chart that combines all given genes
// */
//QChartView * createBarChart(std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> geneExpressionValuesForClusters, QString plotTitle) {

//    // Store the highest expression value for the range of the yAxis
//    double maxExpressionValue = .0;
////    int numberOfClusters = std::get<1>(geneExpressionValuesForClusters.first()).length();

//    QBarSeries * barSeries = new QBarSeries();

//    // Go through all clusters and create a new Barset for each gene
//    // Attention: One Barseries is made per gene, not per cluster, for infos go to: https://doc.qt.io/qt-5/qbarset.html
//    for (int i = 0; i < std::get<0>(geneExpressionValuesForClusters).length(); i++) {
//        QString barSetTitle = std::get<0>(std::get<0>(geneExpressionValuesForClusters).at(i)).toUpper() + " - ⌀: "
//                + QString::number(std::get<2>(std::get<0>(geneExpressionValuesForClusters).at(i)));
//        QBarSet * barSet = new QBarSet(barSetTitle);

//        // Add all expression values for the current gene
//        for (int j = 0; j < std::get<1>(std::get<0>(geneExpressionValuesForClusters).at(i)).length(); j++) {
//            double geneExpressionValueForCluster = std::get<1>(std::get<0>(geneExpressionValuesForClusters).at(i)).at(j);

//            barSet->append(geneExpressionValueForCluster);

//            // Compare to get the max gene expression value for the range of the y axis
//            if (geneExpressionValueForCluster > maxExpressionValue)
//                maxExpressionValue = geneExpressionValueForCluster;
//        }

//        barSeries->append(barSet);
//    }

//    // Create the chart that will hold the above created plotting series
//    QChart * chart = new QChart();
//    chart->addSeries(barSeries);
//    chart->setTitle("Gene expression in " + plotTitle + " clusters");

//    // Create the x axis that will hold the above category names
//    QBarCategoryAxis * xAxis = new QBarCategoryAxis();
//    xAxis->setTitleText(plotTitle + " clusters");
//    xAxis->append(std::get<1>(geneExpressionValuesForClusters));

//    // Create the y axis that will show the gene expression values
//    QValueAxis * yAxis = new QValueAxis();
//    yAxis->setTitleText("relative UMI counts per cell");
//    yAxis->setRange(0, maxExpressionValue + 5);

//    // Add the new axes to the plot and attach them to the plotting series
//    chart->addAxis(xAxis, Qt::AlignBottom);
//    chart->addAxis(yAxis, Qt::AlignLeft);
//    barSeries->attachAxis(xAxis);
//    barSeries->attachAxis(yAxis);

//    // Configure the main chart legend
//    chart->legend()->setVisible(true);
//    chart->legend()->setAlignment(Qt::AlignTop);

//    // Crete the final chart view that will be returned
//    QChartView * chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);

//    return chartView;
//}

}
