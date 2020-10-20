#ifndef PLOTS_H
#define PLOTS_H

#include <QVector>
#include <QPair>
#include <QtCharts/QChartView>

using QtCharts::QChartView;

namespace Plots {

extern QChartView * createScatterPlot(const QString title, const QMap<QString, QVector<double>> expressionDataForGenesInClusters, const QStringList clusterNames, const QVector<double> meanValues);
//extern QChartView * createScatterPlot(std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> geneExpressionValuesForClusters, QString plotTitle);
extern QChartView * createBarChart(std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> geneExpressionValuesForClusters, QString plotTitle);

};

#endif // PLOTS_H
