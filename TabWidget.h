#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
#include <QTableWidgetItem>
#include <QStringList>
#include <QObject>
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
    explicit TabWidget(QWidget *parent = nullptr, QString title = "");
    ~TabWidget();

    void populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, int numberOfItems);
    void populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs);

    QVector<FeatureCollection> retrieveAllSeenData();

private slots:
    void on_lineEditGeneID_textChanged(const QString &arg1);

//    void on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column);

    void on_pushButtonScatterPlot_clicked();
    void on_pushButtonBarChart_clicked();

    // MIN RAW COUNT
    void on_spinBoxFilterOptionsRawCountCutOffMin_valueChanged(int value);
    void on_horizontalSliderFilterOptionsRawCountCutOffMin_sliderMoved(int position);

    // MAX RAW COUNT
    void on_spinBoxFilterOptionsRawCountCutOffMax_valueChanged(int value);
    void on_horizontalSliderFilterOptionsRawCountCutOffMax_sliderMoved(int position);

    // MIN FOLD CHANGE
    void on_spinBoxFilterOptionsFoldChangeCutOffMin_valueChanged(int value);
    void on_horizontalSliderFilterOptionsFoldChangeCutOffMin_sliderMoved(int position);

    // MAX FOLD CHANGE
    void on_spinBoxFilterOptionsFoldChangeCutOffMax_valueChanged(int value);
    void on_horizontalSliderFilterOptionsFoldChangeCutOffMax_sliderMoved(int position);

    // IN AT LEAST
    void on_checkBoxFilterOptionsRawCountCutOffInAtLeast_toggled(bool checked);
    void on_checkBoxFilterOptionsFoldChangeCutOfftInAtLeast_toggled(bool checked);
    void on_spinBoxFilterOptionsRawCountCutOffInAtLeast_valueChanged(int number);
    void on_spinBoxFilterOptionsFoldChangeCutOffInAtLeast_valueChanged(int number);

private:
    Ui::TabWidget *ui;
    QString title;

    QTableView * tableView;
    GeneTableModel * geneTableModel;
    ProxyModel * proxyModel;

    double minRawCount;
    double maxRawCount;
    double minFoldChange;
    double maxFoldChange;

    template<typename F>
    void openExportWidgetWithPlot(F plottingFunction);
    void cleanCorrelationTable();
    void setMaxValuesForGUIElements(const double highestMetRawCount, const double highestMetFoldChange);
    std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> retrieveExpressionDataForSelectedGenes();

};

#endif // TABWIDGET_H
