#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProxyModel(QObject * parent = nullptr);
    ProxyModel(int rowCount, int colCount, QObject * parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void setMinRawCount(double minRawCount);
    void setMaxRawCount(double maxRawCount);
    void setMinFoldChange(double minFoldChange);
    void setMaxFoldChange(double maxFoldChange);
    void setRawCountInAtLeast(int amount);
    void setFoldChangeInAtLeast(int amount);

private:
    int rowCount;
    int columnCount;

    double minRawCount;
    double maxRawCount;
    double minFoldChange;
    double maxFoldChange;
    double rawCountinAtLeast;
    double foldChangeInAtLeast;
};

#endif // PROXYMODEL_H
