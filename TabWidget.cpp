#include <QTableWidgetItem>
#include <QDebug>

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


void TabWidget::on_newDatasetTabItemCreated(const QString datasetName, const QVector<QVector<QPair<QString, double>>> correlation) {
    int numberOfRows = correlation.length(), // Number of clusters
        numberOfColumns = correlation[0].length(); // Number of features used in correlation

    this->ui->tableWidgetTypeCorrelations->setRowCount(numberOfRows);
    this->ui->tableWidgetTypeCorrelations->setColumnCount(numberOfColumns);

    QStringList horizontalHeaderItems;
    for (int i = 0; i < numberOfRows; i++) {
        horizontalHeaderItems.append("Cluster" + QString::number(i));
        this->ui->tableWidgetTypeCorrelations->insertRow(0);
    }

    QStringList verticalHeaderItems;
    for (QPair<QString, double> correlatedFeature : correlation[0]) {
        verticalHeaderItems.append(correlatedFeature.first);
        this->ui->tableWidgetTypeCorrelations->insertColumn(0);
    }

    this->ui->tableWidgetTypeCorrelations->setHorizontalHeaderLabels(horizontalHeaderItems);
    this->ui->tableWidgetTypeCorrelations->setHorizontalHeaderLabels(verticalHeaderItems);

    QTableWidgetItem * qTableWidgetItem = new QTableWidgetItem(0);
    this->ui->tableWidgetTypeCorrelations->setItem(0, 0, qTableWidgetItem);

    this->ui->tableWidgetTypeCorrelations->setSortingEnabled(true);
}


void TabWidget::populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, int numberOfItems) {
    int numberOfClusters = correlations.length(); // Number of clusters

    this->ui->tableWidgetTypeCorrelations->setColumnCount(numberOfClusters);
    this->ui->tableWidgetTypeCorrelations->setRowCount(numberOfItems);

    QStringList horizontalHeaderItems;
    for (int i = 1; i < numberOfClusters + 1; i++) {
        horizontalHeaderItems.append("Cluster" + QString::number(i));
    }

    this->ui->tableWidgetTypeCorrelations->setHorizontalHeaderLabels(horizontalHeaderItems);

    for (int i = 0; i < correlations.length(); i++) {
        qDebug() << "CLUSTER " << i << ":";
        for (int j = 0; j < numberOfClusters; j++) {
            QPair<QString, double> type = correlations[i][j];
            QString cell = QString::number(type.second) + ": " + type.first;

            QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
            tableWidgetItem->setData(Qt::DisplayRole, cell);

            this->ui->tableWidgetTypeCorrelations->setItem(i, j, tableWidgetItem);
            qDebug() << correlations[i][j].first << ":" << correlations[i][j].second;
        }
    }
}
