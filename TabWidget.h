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

private slots:
//    void on_lineEditGeneID_textChanged(const QString &arg1);

//    void on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column);

//    void on_pushButtonScatterPlot_clicked();

//    void on_pushButtonBarChart_clicked();

//    void on_pushButtonCorrelationOptionsRun_clicked();

private:
    Ui::TabWidget *ui;
    QString title;

    QTableView * tableView;
    GeneTableModel * geneTableModel;
    ProxyModel * proxyModel;

//    QVector<QPair<QString, QPair<QVector<double>, double>>> retrieveExpressionDataForSelectedGenes();

    void showAlertForInvalidGeneID(QString geneID);

    template<typename F>
    void openExportWidgetWithPlot(F plottingFunction);

    const char lineEditDelimiter = ',';
};

#endif // TABWIDGET_H
