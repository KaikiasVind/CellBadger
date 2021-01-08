#include "TSNEDialog.h"
#include "ui_TSNEDialog.h"

TSNEDialog::TSNEDialog(QWidget *parent) :
    ExportDialog(parent),
    ui(new Ui::TSNEDialog)
{
    ui->setupUi(this);
}

TSNEDialog::~TSNEDialog()
{
    delete ui;
}
