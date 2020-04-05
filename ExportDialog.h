#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

#include "QtCharts/QChartView"

using QtCharts::QChartView;

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    ~ExportDialog();

    void addPlot(QChartView * chartView);

private:
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
