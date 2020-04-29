#ifndef PLOTS_H
#define PLOTS_H

#include <QVector>
#include <QPair>
#include <QtCharts/QChartView>

using QtCharts::QChartView;

namespace Plots {

extern QChartView * createScatterPlot(QVector<QPair<QString, QPair<QVector<double>, double>>> geneExpressionValuesForClusters, QString plotTitle);
extern QChartView * createBarChart(QVector<QPair<QString, QPair<QVector<double>, double>>> geneExpressionValuesForClusters, QString plotTitle);

};

#endif // PLOTS_H