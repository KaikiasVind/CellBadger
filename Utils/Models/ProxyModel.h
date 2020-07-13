#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProxyModel(QObject * parent = nullptr);
    ProxyModel(int rowCount, int colCount, double maxRawCount, double maxFoldChange, QObject * parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

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
