#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
#include <QTableWidgetItem>
#include <QStringList>
#include <QObject>

#include "BioModels/FeatureCollection.h"

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
    void on_lineEditGeneID_textChanged(const QString &arg1);

    void on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column);

    void on_pushButtonScatterPlot_clicked();

    void on_pushButtonBarChart_clicked();

private:
    Ui::TabWidget *ui;

    QString title;

    const char lineEditDelimiter = ',';

    QVector<QPair<QString, QVector<double>>> retrieveExpressionDataForSelectedGenes();

    void showAlertForInvalidGeneID(QString geneID);

    template<typename F>
    void openExportWidgetWithPlot(F plottingFunction);
};

#endif // TABWIDGET_H
