#ifndef ANALYSISTAB_H
#define ANALYSISTAB_H

#include <QWidget>

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

private:
    Ui::AnalysisTab *ui;
};

#endif // ANALYSISTAB_H
