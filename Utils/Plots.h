#ifndef PLOTS_H
#define PLOTS_H

#include <QVector>
#include <QPair>
#include <QtCharts/QChartView>

#include "AnalysisTab.h"

using QtCharts::QChartView;

namespace Plots {

extern QChartView * createScatterPlot(const QString title, const QString yAxisTitle, const QMap<QString, QVector<double>> expressionDataForGenesInClusters, const QStringList clusterNames, const QVector<double> meanValues = {});
//extern QChartView * createBarChart(std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> geneExpressionValuesForClusters, QString plotTitle);
extern QChartView * createUMAPPlot(const QString title, const QVector<std::tuple<QString, int, double, double>> tSNEProjectionData, AnalysisTab * analysisTab, const QStringList clusterTypes);


};

#endif // PLOTS_H
