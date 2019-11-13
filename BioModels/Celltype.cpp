#include "Celltype.h"

CellType::CellType(QString associatedTissueType, QVector<QString> associatedMarkers)
    : m_associatedTissueType {associatedTissueType}, m_associatedMarkers {associatedMarkers}
{}
