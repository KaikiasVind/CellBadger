#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "Utils/Helper.h"

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProxyModel(QObject * parent = nullptr);
    ProxyModel(int rowCount, int colCount, double maxRawCount, double maxFoldChange, QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashedGeneDataForAllClusters, QObject * parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setCurrentlyShownDataType(const Helper::ShownData newDataTypeToShow);

    void setMinRawCount(double minRawCount);
    void setMaxRawCount(double maxRawCount);
    void setMinFoldChange(double minFoldChange);
    void setMaxFoldChange(double maxFoldChange);
    void setRawCountInAtLeast(int amount);
    void setIncludeRawCountInAtLeast(bool state);
    void setFoldChangeInAtLeast(int amount);
    void setIncludeFoldChangeInAtLeast(bool state);
    void setSearchedGeneIDs(QStringList searchedGeneIDs);

private:
    int rowCount;
    int columnCount;

    Helper::ShownData currentlyShownDataType;
    QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashedGeneDataForAllClusters;

    double minRawCount;
    double maxRawCount;
    double minFoldChange;
    double maxFoldChange;
    int rawCountinAtLeast;
    bool includeRawCountInAtLeast;
    int foldChangeInAtLeast;
    bool includeFoldChangeInAtLeast;
    QStringList searchedGeneIDs;
};

#endif // PROXYMODEL_H
