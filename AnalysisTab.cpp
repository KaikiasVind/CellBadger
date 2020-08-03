#include "AnalysisTab.h"
#include "ui_AnalysisTab.h"

#include <QDebug>

AnalysisTab::AnalysisTab(QWidget *parent) :
    QWidget(parent), ui(new Ui::AnalysisTab)
{

    ui->setupUi(this);
}

AnalysisTab::~AnalysisTab() {
    delete ui;
}


void AnalysisTab::addExperiment(const QString experimentName, const QVector<FeatureCollection> experiment) {

    // Make room for one more column
    this->ui->tableWidgetExperimentsSelection->setColumnCount(this->ui->tableWidgetExperimentsSelection->columnCount() + 1);

    // And add additional rows in case the experiment has more clusters than previously added experiments
    if (this->ui->tableWidgetExperimentsSelection->rowCount() < experiment.length())
        this->ui->tableWidgetExperimentsSelection->setRowCount(experiment.length());

    int currentNumberOfHeaderItems = this->ui->tableWidgetExperimentsSelection->columnCount();

    qDebug() << "header items:" << currentNumberOfHeaderItems;
    // Create a new header item for the new experiment and add it to the horizontal header
    QTableWidgetItem * newHeaderItem = new QTableWidgetItem(0);
    newHeaderItem->setData(Qt::DisplayRole, experimentName);
    this->ui->tableWidgetExperimentsSelection->setHorizontalHeaderItem(currentNumberOfHeaderItems - 1, newHeaderItem);

    // Add every cluster to the table
    for (int i = 0; i < this->ui->tableWidgetExperimentsSelection->rowCount(); i++) {
        QTableWidgetItem * newTableItem = new QTableWidgetItem(0);
        newTableItem->setData(Qt::DisplayRole, experiment.at(i).ID);
        this->ui->tableWidgetExperimentsSelection->setItem(i, currentNumberOfHeaderItems - 1, newTableItem);
    }
}
