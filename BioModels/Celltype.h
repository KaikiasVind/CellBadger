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

    CellType(QString ID, QString associatedTissueType, QStringList associatedMarkers);
};

#endif // CELLTYPE_H
