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

    CSVReader csvReader;
    
    // CUTOFF IS COMMENTED OUT IN FUNCTION!!!
//    QVector<Cluster> clusterExpressions = csvReader.getClusterFeatureExpressions(clusterExpressionFilePath, 15);
    QVector<Cluster> clusterExpressions = csvReader.getClusterFeatureExpressions(clusterExpressionFilePath);

    // Parse cell- / tissue type makers csv file and return all cell types with associated markers
//    QVector<QPair<QPair<QString, QString>, QStringList>> cellTypeMarkers = csvReader.getCellTypeMarkers(cellMarkersFilePath);
    QVector<CellType> cellTypeMarkers = csvReader.getCellTypesWithMarkers(cellMarkersFilePath);

    ExpressionComparator expressionComparator;
//    QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods = expressionComparator.findCellTypeCorrelations(cellTypeMarkers, clusterExpressions);
    QVector<QVector<QPair<CellType, double>>> clustersWithCellTypeMappingLikelihoods = expressionComparator.findCellTypeCorrelations(cellTypeMarkers, clusterExpressions);
    // cluster : clustersWithCellTypeMappingLikelihoods

    Sorter::findHighestLikelyCellTypeMapping(clustersWithCellTypeMappingLikelihoods);
    
    return a.exec();
}
