#include "PlotButton.h"

#include <QPushButton>

PlotButton::PlotButton(const QString & text, QWidget * parent, const int buttonIndex) :
    QPushButton(text, parent),
    buttonIndex(buttonIndex)
{
    connect(this, &PlotButton::clicked, this, &PlotButton::on_clicked);
}

PlotButton::~PlotButton() {};

// ###### SLOTS ######

/**
 * @brief PlotButton::on_clicked - Reroute the clicked signal to report the button index
 * @param checked - UNUSED
 */
void PlotButton::on_clicked(bool checked) {
    Q_UNUSED(checked);
    emit this->clickedWithIndex(this->buttonIndex);
}
