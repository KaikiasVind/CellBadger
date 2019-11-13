#include "Mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDebug>

#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#include "Utils/Sorter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    // Parse cluster expression file and return all clusters with expressed features for each cluster
    // Human
    QString clusterExpressionFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/10xGenomics/Downloads/5k_Pbmc/analysis/diffexp/graphclust/differential_expression.csv";
    QString cellMarkersFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/Biologische Daten/Human_cell_markers.csv";

    // Mus Musclus
//    QString clusterExpressionFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/10xGenomics/Downloads/5k_Neuron/analysis/diffexp/graphclust/differential_expression.csv";
//    QString cellMarkersFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/Biologische Daten/Mouse_cell_markers.csv";

    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++
    QString configFilePath = "/home/numelen/.badger.conf";
    ConfigFileOperator configFileOperator;

    bool isConfigFileExits = configFileOperator.isConfigFileExists(configFilePath);

    if (isConfigFileExits) {
        qDebug() << "Reading config file:" << configFilePath;
        configFileOperator.readConfigFile(configFilePath);
        cellMarkersFilePath = configFileOperator.getCellMarkersFilePath();
        clusterExpressionFilePath = configFileOperator.getClusterExpressionFilePath();
    } else {
        qDebug() << "No config file found.";
//        QFileDialog fileDialog;
//        QFile cellMarkersFile = fileDialog.setFileMode(QFileDialog::AnyFile);
    }
    // ++++++++++++++++++++++++ CHECK FOR CONFIG FILE +++++++++++++++++++++++++++++

    CSVReader csvReader;
    
    QVector<QStringList> clusterExpressions = csvReader.getClusterFeatureExpressions(clusterExpressionFilePath, 15);
    
    // Parse cell- / tissue type makers csv file and return all cell types with associated markers
    QVector<QPair<QPair<QString, QString>, QStringList>> cellTypeMarkers = csvReader.getCellTypeMarkers(cellMarkersFilePath);
    
    ExpressionComparator expressionComparator;
    QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods = expressionComparator.findCellTypeCorrelations(cellTypeMarkers, clusterExpressions);
    
    Sorter sorter;
    sorter.findHighestLikelyCellTypeMapping(clustersWithCellTypeMappingLikelihoods);
    
    return a.exec();
}
