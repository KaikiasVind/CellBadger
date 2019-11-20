#include "Mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#include "Utils/Sorter.h"
#include "BioModels/FeatureCollection.h"

using namespace CSVReader;
using namespace ExpressionComparator;
using namespace Sorter;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //    w.show();
    
    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++
    QString configFilePath = QDir::homePath().append("/.badger.conf");
    ConfigFileOperator configFileOperator;

    bool isConfigFileExits = configFileOperator.isConfigFileExists(configFilePath);

    QString cellMarkersFilePath,
            clusterExpressionFilePath;

    if (isConfigFileExits) {
        qDebug() << "Reading config file:" << configFilePath;
        configFileOperator.readConfigFile(configFilePath);
        cellMarkersFilePath = configFileOperator.getCellMarkersFilePath();
        clusterExpressionFilePath = configFileOperator.getClusterExpressionFilePath();
    } else {
        qDebug() << "No config file found under:" << configFilePath;
        exit(1);
    }
    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++

//    // Parse cell- / tissue type makers csv file and return all cell types with associated markers
//    QVector<CellType> cellTypeMarkers = CSVReader::getCellTypesWithMarkers(cellMarkersFilePath);

//    QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods = ExpressionComparator::findCellTypeCorrelations(cellTypeMarkers, clusterExpressions);

//    Sorter::findHighestLikelyCellTypeMapping(clustersWithCellTypeMappingLikelihoods);
    
    qDebug() << "Reading marker expression file.";
    QVector<FeatureCollection> tissues = CSVReader::getTissuesWithGeneExpression(cellMarkersFilePath);
    qDebug() << "Done";

    qDebug() << "Reading 10x cluster expression file.";
    QVector<FeatureCollection> xClusterGeneExpressions = CSVReader::getClusterFeatureExpressions(clusterExpressionFilePath, 15);
    qDebug() << "Done";

    QVector<FeatureCollection> rankedClusterGeneExpressions;
    QVector<FeatureCollection> rankedTissueGeneExpressions;
    qDebug() << "Ranking cluster features";
    for (FeatureCollection collection : xClusterGeneExpressions)
        rankedClusterGeneExpressions.append(Sorter::rankFeaturesByExpression(collection));
    qDebug() << "Done";
    qDebug() << "Ranking tissue features";
    for (FeatureCollection collection : tissues)
        rankedTissueGeneExpressions.append(Sorter::rankFeaturesByExpression(collection));
    qDebug() << "Done";


    return a.exec();
}
