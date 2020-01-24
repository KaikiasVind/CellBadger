#include "Celltype.h"

#include <QString>
#include <QStringList>

CellType::CellType(){};

/**
 * @brief CellType::CellType - Container class for cell / tissue - marker association
 * @param associatedTissueType - ID of the cell's tissue type from database
 * @param associatedMarkers - List of cell-markers expressed by this cell type
 */
CellType::CellType(QString cellTypeID, QString associatedTissueTypeForCellType, QStringList associatedMarkersForCellType)
    : ID {cellTypeID}, associatedTissueType {associatedTissueTypeForCellType}, associatedMarkers {associatedMarkersForCellType}
{}

