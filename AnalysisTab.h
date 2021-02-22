#ifndef ANALYSISTAB_H
#define ANALYSISTAB_H

#include <QWidget>
#include <QStringList>

#include "BioModels/FeatureCollection.h"
#include "ScatterSeries.h"

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
    void requestDEAnalysis(const QString matrixFilePath, const QString clusteringInformationFilePath, const QVector<int> clustersToCompare);

public slots:
    void on_receivedGeneExpressionData(const QVector<QVector<FeatureCollection>> experiments, const QVector<QStringList> completeSetsOfGeneIDsPerDataset);
    void on_receivedDEAnalysisData(const QVector<QPair<QString, QVector<double>>> differentiallyExpressedGenes);
    void on_plotButtonClicked(const int buttonIndex);
    void on_lineSeriesClickedWithIndex(const int index, ScatterSeries const *series);

private slots:
    void on_pushButtonScatterPlot_clicked();

    void on_pushButtonBarChart_clicked();

    void on_pushButtonDEAnalysisAnalyze_clicked();

    void on_pushButtonDEAnalysisExport_clicked();

private:
    Ui::AnalysisTab *ui;

    enum class PlotType {
        SCATTER_PLOT
    };

    PlotType requestedPlotType;

    void addPlotButtonItemToTable(const int columnIndex);
    QVector<int> getSelectedClusters();
};

#endif // ANALYSISTAB_H
