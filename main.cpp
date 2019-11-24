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

    qDebug() << "Reading 10x cluster expression file.";
    QVector<FeatureCollection> xClusterGeneExpressions = CSVReader::getClusterFeatureExpressions(clusterExpressionFilePath, 15);
    qDebug() << "Done";

    qDebug() << "Reading marker expression file.";
    QVector<FeatureCollection> tissues = CSVReader::getTissuesWithGeneExpression(cellMarkersFilePath, 100);
    qDebug() << "Done";

    qDebug() << xClusterGeneExpressions[0].getNumberOfFeatures() << ":" << tissues[0].getNumberOfFeatures();

    qDebug() << "Finding cluster - tissue correlations";
    QVector<QVector<QPair<QString, double>>> clustersWithTissueCorrelations;
    clustersWithTissueCorrelations.reserve(xClusterGeneExpressions.length());
    clustersWithTissueCorrelations = ExpressionComparator::findClusterTissueCorrelations(xClusterGeneExpressions, tissues);
    qDebug() << "Done";

//    int i = 0;
//    for (QVector<QPair<QString, double>> clusterWithTissueCorrelations : clustersWithTissueCorrelations) {
//        qDebug() << "cluster:" << i++;
//        for (QPair<QString, double> correlation : clusterWithTissueCorrelations) {
//            qDebug() << correlation.first << ":" << correlation.second;
//        }
//        qDebug() << "\n";
//    }

//    for (QPair<QString, double> pair : clusterTissueCorrelations) {
//        qDebug() << pair.first << ":" << pair.second;
//    }
//    qDebug() << "\n";

//    qDebug() << "Finding equally expressed Features";
//    QVector<QPair<Feature, Feature>> equallyExpressedFeatures = Sorter::findEquallyExpressedFeatures(tissues[0], xClusterGeneExpressions[0]);

//    qDebug() << "Equally expressed features:";
//    for (int i = 0; i < equallyExpressedFeatures.length(); i++) {
//        qDebug() << equallyExpressedFeatures[i].first.ID << ":" << equallyExpressedFeatures[i].first.count << "-" <<
//                    equallyExpressedFeatures[i].second.ID << ":" << equallyExpressedFeatures[i].second.count;
//    }

//    QVector<double> tissueFeatureExpressions = tissues[0].getMostExpressedFeaturesCounts(200);
//    QVector<double> clusterFeatureExpressions = xClusterGeneExpressions[0].getMostExpressedFeaturesCounts(200);

//    qDebug() << tissueFeatureExpressions << "\n" << clusterFeatureExpressions;
//    qDebug() << "Result:" << Correlator::calculateSpearmanCorrelation(tissueFeatureExpressions, clusterFeatureExpressions);

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
