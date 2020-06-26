#include "GeneTableModel.h"

#include <QDebug>

#include "BioModels/FeatureCollection.h"


GeneTableModel::GeneTableModel(const QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs, QObject * parent):
    QAbstractTableModel(parent)
{
    this->geneExpressions = geneExpressions;
    this->completeGeneIDs = completeGeneIDs;
}


/**
 * @brief GeneTableModel::rowCount - Fetch the number of rows that the gene expression table shows,
 * @param parent - Parent widget (unused)
 * @return - Number of rows of the table (number of genes + title row)
 */
int GeneTableModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return this->completeGeneIDs.length() + 1;
}


/**
 * @brief GeneTableModel::columnCount - Fetch the number of columns the gene expressions shows.
 * @param parent - Parent widget (unused)
 * @return - Number of columns of the table (number of clusters + gene id column + mean count column)
 */
int GeneTableModel::columnCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return this->geneExpressions.length() + 2;
}


/**
 * @brief GeneTableModel::data - Influences how the data is presented in the table.
 * @param index - Index of the current cell.
 * @param role - Information type that is requested currently, e.g. content or alignment of the cell
 * @return - QVariant with the correct way the data should be presented.
 */
QVariant GeneTableModel::data(const QModelIndex & index, int role) const {

    // Block invalid indices
    if (!index.isValid()) {
        qDebug() << "Invalid";
        return QVariant();
    }

    if (index.row() >= this->rowCount() || index.row() < 0) {
        qDebug() << "< >";
        return QVariant();
    }

    // Fetch the data from the underlying data models and report it to the table
    if (role == Qt::DisplayRole) {
//        const FeatureCollection & featureCollection = geneExpressions.at(index.row());
        QVector<double> geneExpressionCountsInAllClusters;
        geneExpressionCountsInAllClusters.reserve(this->geneExpressions.length());
        for (int i = 0; i < this->geneExpressions.length(); i++) {
            geneExpressionCountsInAllClusters.append(this->geneExpressions.at(i).getFeatureExpressionCount(index.row()));
        }

        if (index.column() == 0) {
            qDebug() << "gene.";
        } else if (index.column() == this->geneExpressions.length() + 1) {
            qDebug() << "mean";
        } else {
            qDebug() << "cell.";
        }

//            qDebug() << "column:" << index.column();
//            qDebug() << "row:" << index.row();
//            qDebug() << index.column() << ":" << this->completeGeneIDs.at(index.column());
//            return 0;

//            return this->completeGeneIDs.at(index.row());
//        } else {
//            qDebug() << index.column() << ": 1";
//            return 1;
//            Feature feature = this->geneExpressions.at(index.column()).getFeature(index.row());
//            qDebug() << index.column() << "-" << this->geneExpressions.at(index.column()).ID << ":" <<
//                        feature.ID << "-" << feature.count;
//            return 1;
//            return this->geneExpressions.at(index.column()).getFeatureExpressionCount(index.row());
//        }

//        if (index.column() == 0) {
//            qDebug() << "0:" << featureCollection.getFeatureID(index.row());
//            return featureCollection.getFeatureID(index.row());
////        } else if (index.column() == this->columnCount() - 1) {
////            qDebug() << index.column() << "sum:" << featureCollection.getExpressionCountSum();
////            return featureCollection.getExpressionCountSum();
//        } else {
//            qDebug() << index.column() << ":" << featureCollection.getFeatureRawCount(index.column());
//            return featureCollection.getFeatureRawCount(index.column());
//        }

    // Decide which cell should be aligned in which way
    } else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0 || index.column() == this->columnCount() - 1)
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        else
            return Qt::AlignCenter;
    }

    return QVariant();
}


/**
 * @brief GeneTableModel::headerData - Set the header labels
 * @param section - Current vertical header cell index
 * @param orientation - Orientation
 * @param role - Role
 * @return - The label that the current vertical header cell should show
 */
QVariant GeneTableModel::headerData(int section, Qt::Orientation orientation, int role) const {

    // Only care about display options here
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Gene");
        } else if (section == this->geneExpressions.length() + 1) {
            return tr("mean");
        } else {
            return tr(qPrintable(this->geneExpressions.at(section - 1).ID));
        }
    }

    return section + 1;
}

