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

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonBack_clicked();

private:
    Ui::ExportDialog *ui;

    QChartView * chartView;

    void closeEvent(QCloseEvent *);

    void hideAndClean();
};

#endif // EXPORTDIALOG_H
