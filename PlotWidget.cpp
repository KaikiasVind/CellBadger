#include "PlotWidget.h"
#include "ui_PlotWidget.h"

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
}

PlotWidget::~PlotWidget()
{
    delete ui;
}
