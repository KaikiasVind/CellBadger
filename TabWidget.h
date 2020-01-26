#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>

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

public slots:
    void on_newDatasetTabItemCreated(const QString datasetName, const QVector<QVector<QPair<QString, double>>> correlation);

private:
    Ui::TabWidget *ui;
};

#endif // TABWIDGET_H
