#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "Utils/Definitions.h"

using Definitions::ShownData;

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProxyModel(QObject * parent = nullptr);
    ProxyModel(int rowCount, int colCount, double maxRawCount, double maxFoldChange, QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashedGeneDataForAllClusters, QObject * parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setCurrentlyShownDataType(const ShownData newDataTypeToShow);
    void setSearchedGeneIDs(QStringList searchedGeneIDs);

    void setMinRPM(const double minRPM);
    void setMaxRPM(const double maxRPM);
    void setIncludeRPMInAtLeast(const bool includeRPMInAtLeast);
    void setRPMInAtLeast(const int rpmInAtLeast);

    void setMinRawCount(const double minRawCount);
    void setMaxRawCount(const double maxRawCount);
    void setRawCountInAtLeast(const int amount);
    void setIncludeRawCountInAtLeast(const bool state);

    void setMinFoldChange(const double minFoldChange);
    void setMaxFoldChange(const double maxFoldChange);
    void setFoldChangeInAtLeast(const int amount);
    void setIncludeFoldChangeInAtLeast(const bool state);

private:
    int rowCount;
    int columnCount;

    ShownData currentlyShownDataType;
    QMap<QString, std::tuple<QVector<double>, QVector<double>, QVector<double>>> hashedGeneDataForAllClusters;

    double minRPM;
    double maxRPM;
    int rpmInAtLeast;
    bool includeRPMInAtLeast;

    double minRawCount;
    double maxRawCount;
    int rawCountinAtLeast;
    bool includeRawCountInAtLeast;

    double minFoldChange;
    double maxFoldChange;
    int foldChangeInAtLeast;
    bool includeFoldChangeInAtLeast;

    QStringList searchedGeneIDs;
};

#endif // PROXYMODEL_H
