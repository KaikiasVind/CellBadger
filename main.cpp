#include "Mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#include "Utils/Sorter.h"
#include "BioModels/Cluster.h"

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
        //        QFileDialog fileDialog;
        //        QFile cellMarkersFile = fileDialog.setFileMode(QFileDialog::AnyFile);
    }
    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++

    // CUTOFF IS COMMENTED OUT IN FUNCTION!!!
    qDebug() << "Reading cluster expression data.";
    QVector<Cluster> clusterExpressions = CSVReader::getClusterFeatureExpressions(clusterExpressionFilePath, 15);
//    QVector<Cluster> clusterExpressions = csvReader.getClusterFeatureExpressions(clusterExpressionFilePath);

    // Parse cell- / tissue type makers csv file and return all cell types with associated markers
    QVector<CellType> cellTypeMarkers = CSVReader::getCellTypesWithMarkers(cellMarkersFilePath);

    QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods = ExpressionComparator::findCellTypeCorrelations(cellTypeMarkers, clusterExpressions);

    Sorter::findHighestLikelyCellTypeMapping(clustersWithCellTypeMappingLikelihoods);
    
    return a.exec();
}
