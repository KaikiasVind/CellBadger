#include "ScatterSeries.h"

ScatterSeries::ScatterSeries(const int index)
    : index(index)
{
    QObject::connect(this, &ScatterSeries::clicked, this, &ScatterSeries::on_clicked);
}

// ############################## PRIVATE SLOTS ##############################
/**
 * @brief ScatterSeries::on_clicked
 */
void ScatterSeries::on_clicked() {
    emit this->clickedWithIndex(this->index, this);
}
