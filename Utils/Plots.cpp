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
 * @param geneExpressionValuesForClusters - List of genes with corresponding expression data for n clusters
 * @param plotTitle - The title the resulting plot should show
 * @return - A plot that combines one plotting series per given gene in one scatter plot
 */
QtCharts::QChartView * createScatterPlot(std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> geneExpressionValuesForClusters, QString plotTitle) {
    QChart * chart = new QChart();

    // Collect the highest expression value for the correct y axis range
    double maxExpressionValue = .0;

    int numberOfClusters = std::get<1>(std::get<0>(geneExpressionValuesForClusters).first()).length();

    for (std::tuple<QString, QVector<double>, double> selectedGene : std::get<0>(geneExpressionValuesForClusters)) {
        QScatterSeries * scatterSeries  = new QScatterSeries();
        scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        scatterSeries->setMarkerSize(10);

        for (int i = 0; i < std::get<1>(selectedGene).length(); i++) {
            double expressionValue = std::get<1>(selectedGene).at(i);

            scatterSeries->append(i, expressionValue);

            // If the current expression value is higher than the previously max value, store it as new max
            if (expressionValue >= maxExpressionValue)
                maxExpressionValue = expressionValue;
        }

        // Set the scatter series name with mean expression value
        scatterSeries->setName(std::get<0>(selectedGene).toUpper() + " - ⌀: " + QString::number(std::get<2>(selectedGene)));

        // And add the scatter series for the current gene to the chart
        chart->addSeries(scatterSeries);
  }


    // Add basic configuration
    QString title = "Gene expression in " + plotTitle + " clusters";
    chart->setTitle(title);

    // Add the x axis with the cluster numbers
    QBarCategoryAxis * xAxis = new QBarCategoryAxis();
    xAxis->setTitleText(plotTitle + " clusters");
    xAxis->append(std::get<1>(geneExpressionValuesForClusters));

    // Add the y axis with the relative UMI counts.
    // +1 for the range to make the marker for the max value more visible
    QValueAxis * yAxis = new QValueAxis();
    yAxis->setRange(0, maxExpressionValue + 5);

    yAxis->setTickCount(numberOfClusters);
    yAxis->setTitleText("relative UMI counts per cell");

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
 * @brief createBarChart - Creates a bar chart for the given gene expression data - The resulting chart contains one barset per given gene
 * @param geneExpressionValuesForClusters - List of genes with corresponding expression data for n clusters
 * @param plotTitle - The title the resulting plot should show
 * @return - A bar chart that combines all given genes
 */
QtCharts::QChartView * createBarChart(std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> geneExpressionValuesForClusters, QString plotTitle) {

    // Store the highest expression value for the range of the yAxis
    double maxExpressionValue = .0;
//    int numberOfClusters = std::get<1>(geneExpressionValuesForClusters.first()).length();

    QBarSeries * barSeries = new QBarSeries();

    // Go through all clusters and create a new Barset for each gene
    // Attention: One Barseries is made per gene, not per cluster, for infos go to: https://doc.qt.io/qt-5/qbarset.html
    for (int i = 0; i < std::get<0>(geneExpressionValuesForClusters).length(); i++) {
        QString barSetTitle = std::get<0>(std::get<0>(geneExpressionValuesForClusters).at(i)).toUpper() + " - ⌀: "
                + QString::number(std::get<2>(std::get<0>(geneExpressionValuesForClusters).at(i)));
        QBarSet * barSet = new QBarSet(barSetTitle);

        // Add all expression values for the current gene
        for (int j = 0; j < std::get<1>(std::get<0>(geneExpressionValuesForClusters).at(i)).length(); j++) {
            double geneExpressionValueForCluster = std::get<1>(std::get<0>(geneExpressionValuesForClusters).at(i)).at(j);

            barSet->append(geneExpressionValueForCluster);

            // Compare to get the max gene expression value for the range of the y axis
            if (geneExpressionValueForCluster > maxExpressionValue)
                maxExpressionValue = geneExpressionValueForCluster;
        }

        barSeries->append(barSet);
    }

    // Create the chart that will hold the above created plotting series
    QChart * chart = new QChart();
    chart->addSeries(barSeries);
    chart->setTitle("Gene expression in " + plotTitle + " clusters");

    // Create the x axis that will hold the above category names
    QBarCategoryAxis * xAxis = new QBarCategoryAxis();
    xAxis->setTitleText(plotTitle + " clusters");
    xAxis->append(std::get<1>(geneExpressionValuesForClusters));

    // Create the y axis that will show the gene expression values
    QValueAxis * yAxis = new QValueAxis();
    yAxis->setTitleText("relative UMI counts per cell");
    yAxis->setRange(0, maxExpressionValue + 5);

    // Add the new axes to the plot and attach them to the plotting series
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    barSeries->attachAxis(xAxis);
    barSeries->attachAxis(yAxis);

    // Configure the main chart legend
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);

    // Crete the final chart view that will be returned
    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

}
