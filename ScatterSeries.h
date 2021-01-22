#ifndef SCATTERSERIES_H
#define SCATTERSERIES_H

#include <QObject>
#include <QtCharts/QScatterSeries>
using QtCharts::QScatterSeries;

class ScatterSeries : public QScatterSeries
{
    Q_OBJECT

public:
    ScatterSeries(const int index);

signals:
    void clickedWithIndex(const int index, const ScatterSeries * series);

private:
    const int index;

private slots:
    void on_clicked();
};

#endif // SCATTERSERIES_H
