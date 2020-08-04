#include "AnalysisTab.h"
#include "ui_AnalysisTab.h"

#include <QDebug>

AnalysisTab::AnalysisTab(QWidget *parent) :
    QWidget(parent), ui(new Ui::AnalysisTab)
{

    ui->setupUi(this);
    this->ui->tableWidgetExperimentsSelection->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

AnalysisTab::~AnalysisTab() {
    delete ui;
}


void AnalysisTab::addExperiment(const QString experimentName, const QVector<FeatureCollection> experiment) {

    // Insert an empty column for the new values
    this->ui->tableWidgetExperimentsSelection->insertColumn(0);

    // And add additional rows in case the experiment has more clusters than previously added experiments
    if (this->ui->tableWidgetExperimentsSelection->rowCount() < experiment.length())
        this->ui->tableWidgetExperimentsSelection->setRowCount(experiment.length());


    // Create a new header item for the new experiment and add it to the horizontal header
    QTableWidgetItem * newHeaderItem = new QTableWidgetItem(0);
    newHeaderItem->setData(Qt::DisplayRole, experimentName);
    newHeaderItem->setTextAlignment(Qt::AlignCenter);
    this->ui->tableWidgetExperimentsSelection->setHorizontalHeaderItem(0, newHeaderItem);

    // Add every cluster to the table
    for (int i = 0; i < experiment.length(); i++) {
        QTableWidgetItem * newTableItem = new QTableWidgetItem(0);
        newTableItem->setData(Qt::DisplayRole, experiment.at(i).ID);
        newTableItem->setTextAlignment(Qt::AlignCenter);
        this->ui->tableWidgetExperimentsSelection->setItem(i, 0, newTableItem);
    }
}
