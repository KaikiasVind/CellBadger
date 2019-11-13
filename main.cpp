#include "Mainwindow.h"

#include <QApplication>

#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#include "Utils/Sorter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    CSVReader csvReader;
    
    // Referenz checken -> Gibt es eine Überlappung
    // Spearman Rangkorrelation -> Auch für Expressionsdaten interessant
    // Matrix für Heatmap
    
    // Parse cluster expression file and return all clusters with expressed features for each cluster
    // Human
    QString clusterExpressionFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/10xGenomics/Downloads/5k_Pbmc/analysis/diffexp/graphclust/differential_expression.csv";
    QString cellMarkersFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/Biologische Daten/Human_cell_markers.csv";
    
    // Mus Musclus
//    QString clusterExpressionFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/10xGenomics/Downloads/5k_Neuron/analysis/diffexp/graphclust/differential_expression.csv";
//    QString cellMarkersFilePath = "/home/numelen/Documents/Vorlesungen/3-WS_19-20/BA/Biologische Daten/Mouse_cell_markers.csv";
    
    QVector<QStringList> clusterExpressions = csvReader.getClusterFeatureExpressions(clusterExpressionFilePath, 15);
    
    // Parse cell- / tissue type makers csv file and return all cell types with associated markers
    QVector<QPair<QPair<QString, QString>, QStringList>> cellTypeMarkers = csvReader.getCellTypeMarkers(cellMarkersFilePath);
    
    ExpressionComparator expressionComparator;
    QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellTypeMappingLikelihoods = expressionComparator.findCellTypeCorrelations(cellTypeMarkers, clusterExpressions);
    
    Sorter sorter;
    sorter.findHighestLikelyCellTypeMapping(clustersWithCellTypeMappingLikelihoods);
    
    
    return a.exec();
}
