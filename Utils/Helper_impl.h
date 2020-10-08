#ifndef HELPER_IMPL_H
#define HELPER_IMPL_H

#include <QString>
#include <stdio.h>

#include "ExportDialog.h"

namespace Helper {

/**
 * @brief TabWidget::openExportWidgetWithPlot - Ceates a plot with the given plotting function and opens it in an ExportDialog
 * @param plottingFunction - Function that creates a QChartView * that is used to create a plot which is then transfered onto an ExportDialog
 */
template<typename F>
void openExportWidgetWithPlot(F plottingFunction, const QString title, const std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> expressionDataForSelectedGenes) {

    // This case appears if at least one of the gene IDs is not found in the table and therefore is invalid
    if (std::get<0>(expressionDataForSelectedGenes).isEmpty())
        return;

    QChartView * chartView = plottingFunction(expressionDataForSelectedGenes, title);

    ExportDialog * exportDialog = new ExportDialog();
    exportDialog->addPlot(chartView);
    exportDialog->show();
}

}
#endif // HELPER_IMPL_H
