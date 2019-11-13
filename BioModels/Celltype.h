#ifndef CELLTYPE_H
#define CELLTYPE_H

#include <QVector>

struct CellType
{
    const QString m_associatedTissueType;
    const QVector<QString> m_associatedMarkers;

    CellType(QString associatedTissueType, QVector<QString> associatedMarkers);
};

#endif // CELLTYPE_H
