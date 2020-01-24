#ifndef CELLTYPE_H
#define CELLTYPE_H

#include <QString>
#include <QStringList>

/**
 * @brief The CellType struct serves as a container class for cell / tissue - marker association
 */
struct CellType
{
    QString ID;
    QString associatedTissueType;
    QStringList associatedMarkers;

    CellType();
    CellType(const QString ID, const QString associatedTissueType, const QStringList associatedMarkers);
};

#endif // CELLTYPE_H
