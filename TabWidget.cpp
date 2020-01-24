#include "TabWidget.h"
#include "ui_TabWidget.h"

TabWidget::TabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidget)
{
    ui->setupUi(this);
}

TabWidget::~TabWidget()
{
    delete ui;
}
