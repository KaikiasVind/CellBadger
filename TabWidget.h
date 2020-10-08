#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QTableView>

#include "BioModels/FeatureCollection.h"
#include "Utils/Models/GeneTableModel.h"
#include "Utils/Models/ProxyModel.h"

namespace Ui {
class TabWidget;
}

class TabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWidget *parent = nullptr, QString title = "", QStringList clusterNames = QStringList());
    ~TabWidget();

    void populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs);
    void populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, QVector<double> qualityScores, int numberOfItems);
    void populateTableTypeCorrelationHeader(const QVector<double> qualityScores);

    QVector<FeatureCollection> retrieveAllSeenData();

    void setMinRawCount(const double minRawCount);
    void setMaxRawCount(const double maxRawCount);
    void setMinFoldChange(const double minFoldChange);
    void setMaxFoldChange(const double maxFoldChange);
    void setIncludeRawCountInAtLeast(const bool includeRawCountInAtLeast);
    void setIncludeFoldChangeInAtLeast(const bool includeFoldChangeInAtLeast);
    void setRawCountInAtLeast(const int rawCountInAtLeast);
    void setFoldChangeInAtLeast(const int foldChangeInAtLeast);

signals:
    void maxValuesFound(const double highestMetRawCount, const double highestMetFoldChange, const int numberOfClusters);

private slots:
    void on_lineEditGeneID_textChanged(const QString &arg1);

//    void on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column);

    void on_pushButtonScatterPlot_clicked();
    void on_pushButtonBarChart_clicked();
    void on_pushButtonExportCorrelations_clicked();

    void on_pushButtonExportGeneExpressions_clicked();

    void on_comboBoxShownGeneExpressionValues_currentIndexChanged(int index);

private:
    Ui::TabWidget *ui;
    QString title;

    QTableView * tableView;
    GeneTableModel * geneTableModel;
    ProxyModel * proxyModel;

    QStringList clusterNames;

    void cleanCorrelationTable();
    std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> retrieveExpressionDataForSelectedGenes();

    QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashFeatureDataForAllClusters(const QVector<FeatureCollection> experiment, const QStringList completeGeneIDs);
};

#endif // TABWIDGET_H
