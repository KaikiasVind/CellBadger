#ifndef PLOTS_H
#define PLOTS_H

#include <QVector>
#include <QPair>
#include <QtCharts/QChartView>

namespace Plots {

extern QtCharts::QChartView * createScatterPlot(QVector<QPair<QString, QVector<double>>> geneExpressionValuesForClusters, QString plotTitle);
extern QtCharts::QChartView * createBarChart(QVector<QPair<QString, QVector<double>>> geneExpressionValuesForCluster);

};

#endif // PLOTS_H
