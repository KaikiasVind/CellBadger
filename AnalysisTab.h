#ifndef ANALYSISTAB_H
#define ANALYSISTAB_H

#include <QWidget>
#include <QStringList>

#include "BioModels/FeatureCollection.h"

namespace Ui {
class AnalysisTab;
}

class AnalysisTab : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisTab(QWidget *parent = nullptr);
    ~AnalysisTab();

    void addExperiment(const QString experimentName, const QVector<FeatureCollection> experiment, const QVector<QVector<QPair<QString, double>>> correlations);
    void cleanTable();

signals:
    void requestGeneExpressionData();

public slots:
    void on_receivedGeneExpressionData(const QVector<QVector<FeatureCollection>> experiments, const QVector<QStringList> completeSetsOfGeneIDsPerDataset);

private slots:
    void on_pushButtonScatterPlot_clicked();

    void on_pushButtonBarChart_clicked();

private:
    Ui::AnalysisTab *ui;

    enum class PlotType {
        SCATTER_PLOT
    };

    PlotType requestedPlotType;
};

#endif // ANALYSISTAB_H
