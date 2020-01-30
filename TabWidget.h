#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
#include <QTableWidgetItem>

#include "BioModels/FeatureCollection.h"

namespace Ui {
class TabWidget;
}

class TabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWidget *parent = nullptr);
    ~TabWidget();

    void populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, int numberOfItems);

    void populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions);

private slots:
    void on_lineEditGeneID_textChanged(const QString &arg1);

    void on_tableWidgetGeneExpressions_itemDoubleClicked(QTableWidgetItem *item);


private:
    Ui::TabWidget *ui;
};

#endif // TABWIDGET_H
