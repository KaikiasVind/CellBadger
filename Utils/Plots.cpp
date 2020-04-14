#include "Plots.h"

#include <QVector>
#include <QPair>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QCategoryAxis>
#include <QLineSeries>

using QtCharts::QScatterSeries;
using QtCharts::QChart;
using QtCharts::QChartView;
using QtCharts::QValueAxis;
using QtCharts::QBarCategoryAxis;
using QtCharts::QCategoryAxis;
using QtCharts::QLineSeries;

namespace Plots {


/**
 * @brief createScatterPlot - Creates a scatter plot for the given gene expression data - The resulting plot contains one plotting series per given gene
 * @param geneExpressionValuesForClusters - List of genes with corresponding expression data for n clusters
 * @param plotTitle - The title the resulting plot should show
 * @return - A plot that combines one plotting series per given gene in one scatter plot
 */
QtCharts::QChartView * createScatterPlot(QVector<QPair<QString, QVector<double>>> geneExpressionValuesForClusters, QString plotTitle) {
    QChart * chart = new QChart();

    // Collect the highest expression value for the correct y axis range
    double maxExpressionValue = .0;

    int numberOfClusters = geneExpressionValuesForClusters.first().second.length();

    for (QPair<QString, QVector<double>> selectedGene : geneExpressionValuesForClusters) {
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
    QString title = "Gene expression in " + plotTitle + " clusters";
    chart->setTitle(title);

    // Add the x axis with the cluster numbers
    QBarCategoryAxis * xAxis = new QBarCategoryAxis();
    xAxis->setTitleText(plotTitle + " clusters");

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

    return chartView;
}

}
