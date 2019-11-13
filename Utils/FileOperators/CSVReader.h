#ifndef CSVREADER_H
#define CSVREADER_H

#include <QVector>
#include <QStringList>
#include <QString>
#include <QPair>
#include <QHash>

class CSVReader
{
public:
    CSVReader();

    QVector<QStringList> getClusterFeatureExpressions(QString csvFilePath, double cutOff);

    QVector<QPair<QPair<QString, QString>, QStringList>> getCellTypeMarkers(QString csvFilePath);

    QHash <QString, QVector<QPair<QString, QString>>> sortCsvByMarker(QString csvFilePath);
};

#endif // CSVREADER_H
