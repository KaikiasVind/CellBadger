#ifndef GENETABLEMODEL_H
#define GENETABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>

#include "BioModels/FeatureCollection.h"

class GeneTableModel : public QAbstractTableModel
{

public:
    explicit GeneTableModel(QObject * parent = nullptr);
    GeneTableModel(const QVector<std::tuple<QString, QVector<double>, double>> allGenesWithAllExpressionValuesInAllClusters,
                   const QStringList completeGeneIDs, const QStringList allClusterNames, QObject * parent = nullptr);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVector<FeatureCollection> clustersWithGeneExpressions;

private:
    QVector<std::tuple<QString, QVector<double>, double>> allGenesWithExpressionCountsInAllClusters;

    QStringList completeGeneIDs;
    QStringList clusterNames;

    int numberOfClusters;
};

#endif // GENETABLEMODEL_H
