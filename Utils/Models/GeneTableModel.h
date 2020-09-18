#ifndef GENETABLEMODEL_H
#define GENETABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>

#include "BioModels/FeatureCollection.h"
#include "Utils/Definitions.h"

using Definitions::ShownData;

class GeneTableModel : public QAbstractTableModel
{

public:
    explicit GeneTableModel(QObject * parent = nullptr);
    GeneTableModel(const QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashedGeneExpressionData, const QStringList completeGeneIDs, const QStringList allClusterNames, QObject * parent = nullptr);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QStringList getClusterNames() const;

    void setCurrentlyShownDataType(const ShownData dataTypeToShow);

private:
    const QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashedGeneExpressionDataForAllClusters;

    QStringList completeGeneIDs;
    QStringList clusterNames;

    int numberOfClusters;

    ShownData currentlyShownDataType;

    void splitFeaturesIntoValues(const QVector<FeatureCollection> experiment);
};

#endif // GENETABLEMODEL_H
