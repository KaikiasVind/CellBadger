#include "Mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/CSVReader.h"
#include "Utils/Sorter.h"
#include "BioModels/FeatureCollection.h"
#include "Statistics/Expressioncomparator.h"
#include "Statistics/Correlator.h"

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

    qDebug() << "Reading marker expression file.";
    QVector<FeatureCollection> tissues = CSVReader::getTissuesWithGeneExpression(cellMarkersFilePath);
    qDebug() << "Done";

    qDebug() << "Reading 10x cluster expression file.";
    QVector<FeatureCollection> xClusterGeneExpressions = CSVReader::getClusterFeatureExpressions(clusterExpressionFilePath, 15);
    qDebug() << "Done";

    FeatureCollection tissue = tissues[1];
    QVector<double> tissueFeatureExpressions(tissue.getNumberOfFeatures());
    for (int i = 0; i < tissue.getNumberOfFeatures(); i++) {
        tissueFeatureExpressions.append(tissue.getFeatureExpressionCount(i));
    }

    FeatureCollection cluster = xClusterGeneExpressions[0];
    QVector<double> clusterFeatureExpressions(cluster.getNumberOfFeatures());
    for (int i = 0; i < cluster.getNumberOfFeatures(); i++) {
        clusterFeatureExpressions.append(cluster.getFeatureExpressionCount(i));
    }

    qDebug() << tissueFeatureExpressions.length() << ":" << clusterFeatureExpressions.length();

    qDebug() << "Result:" << Correlator::calculateSpearmanCorrelation(tissueFeatureExpressions, clusterFeatureExpressions);

//    QVector<FeatureCollection> rankedClusterGeneExpressions;
//    QVector<FeatureCollection> rankedTissueGeneExpressions;
//    qDebug() << "Ranking cluster features";
//    for (FeatureCollection collection : xClusterGeneExpressions)
//        rankedClusterGeneExpressions.append(Sorter::sortFeaturesByExpression(collection));
//    qDebug() << "Done";
//    qDebug() << "Ranking tissue features";
//    for (FeatureCollection collection : tissues)
//        rankedTissueGeneExpressions.append(Sorter::sortFeaturesByExpression(collection));
//    qDebug() << "Done";

//    QVector<double> variableOne = {59, 35, 43, 23, 42, 27},
//                    variableTwo = {14.61, 11.80, 14.34, 13.03, 14.18, 11.02};


    return a.exec();
}
