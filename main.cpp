#include <QApplication>
#include <QDir>
#include <QObject>

#include "Mainwindow.h"
#include "StartDialog.h"
#include "TabWidget.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "System/Coordinator.h"
#include "System/InformationCenter.h"

#include <QDebug>

//#include "Utils/FileOperators/CSVReader.h"
//#include "Statistics/Expressioncomparator.h"
//#include "Utils/Helper.h"
//#include "Utils/Math.h"

//#include "Utils/Models/AnalysisConfigModel.h"
//#include "Utils/Definitions.h"

#include <RInside.h>
#include <Rcpp.h>
#include <QVector>
#include <stdio.h>
#include <iostream>

#define gui 1

int main(int argc, char * argv[])
{
    QApplication application(argc, argv);

#if !gui

//    QString test("c(1.93888900402828e-284, 1.36707306216849e-231, 1.22184431164881e-223, 2.11918828580732e-222, 2.10559433831497e-216, 4.80123669611008e-215), c(-130.456138906647, -9.88530858758358, -10.2587350113492, -4.07885994940355, -77.1145588361222, -113.196530463417), c(0.662, 0.414, 0.075, 0.046, 0.823, 0.057), c(0.978, 0.917, 0.68, 0.638, 0.973, 0.626), c(4.23046191788931e-280, 2.98281671434543e-227, 2.66594210358653e-219, 4.62385692080299e-218, 4.59419628676944e-212, 1.04758183472426e-210)");

//    QStringList testSplit = test.split("c");

//    std::transform(testSplit.begin(), testSplit.end(), std::back_inserter(testSplit),
//       [](QString string) {
//            // Remove the "(" in the beginning
//            string.remove(0, 1);

//            // And remove the trailing "), " or ")"
//            int chopAtEnd = string.endsWith("), ") ? 3 : 1;
//            string.chop(chopAtEnd);

//            return string;
//        }
//    );

//    QVector<QVector<QPair<QString, QVector<double>>>> dEGeneMAtrixData;

//    for ()

//    for (QString string : testSplit)
//        qDebug() << string << "\n";

//    exit(0);

    QString clusteringInformationFilePath("/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/5k_Pbmc/analysis/clustering/graphclust/clusters.csv");
    QString matrixFilePath("/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/5k_Pbmc/other/filtered_feature_bc_matrix");
    QVector<int> clustersToCompare = {1,2,3};

    // Create the R session
    RInside R;

    // Load the necessary libraries
    R.parseEvalQ("library(Seurat)");
    R.parseEvalQ("library(Matrix)");

    // Assign the given parameters to R variables
    R.assign(clusteringInformationFilePath.toStdString(), "clustering.path");
    R.assign(std::vector<int>(clustersToCompare.begin(), clustersToCompare.end()), "wanted.clusters");
    R.assign(matrixFilePath.toStdString() + "/barcodes.tsv.gz", "barcodes.path");
    R.assign(matrixFilePath.toStdString() + "/features.tsv.gz", "features.path");
    R.assign(matrixFilePath.toStdString() + "/matrix.mtx.gz", "matrix.path");

    // Parse clustering data
    R.parseEvalQ("clustering.data <- read.csv(clustering.path)");

    // Parse and create expression matrix
    R.parseEvalQ("matrix <- readMM(file = matrix.path)");
    R.parseEvalQ("feature.names = read.delim(features.path, header = FALSE, stringsAsFactors = FALSE)");
    R.parseEvalQ("barcode.names = read.delim(barcodes.path, header = FALSE, stringsAsFactors = FALSE)");
    R.parseEvalQ("colnames(matrix) = barcode.names$V1");
    R.parseEvalQ("rownames(matrix) = feature.names$V1");

    // Create and prepare a Seurat object from the given data of the expression matrix
    R.parseEvalQ("seurat <- CreateSeuratObject(counts = matrix, min.cells = 1, min.features = 1)");
    R.parseEvalQ("idx <- match(Cells(seurat), clustering.data$Barcode)");
    R.parseEvalQ("seurat$clustering <- clustering.data$Cluster[idx]");
    R.parseEvalQ("Idents(seurat) <- 'clustering'");

    QString findMarkersEvalString = "FindMarkers(seurat, \
                                                 ident.1 = 1, \
                                                 ident.2 = 2, \
                                                 min.pct = 0.1, \
                                                 logfc.threshold = 0.25, \
                                                 min.diff.pct = 0.1, \
                                                 test.use = 'wilcox', \
                                                 slot = 'data', \
                                                 only.pos = FALSE, \
                                                 verbose = FALSE)";

    // Find the differentially expressed genes for the given clusters
    R.parseEvalQ("DEG <- " + findMarkersEvalString.toStdString());

    // Grab the data frame that was created by the DE analysis
    Rcpp::DataFrame dataFrame = R.parseEval("DEG");

    // Grab the gene ids from the dataframe's row name's
    Rcpp::StringVector geneIDsR = R.parseEval("rownames(DEG)");
    QStringList geneIDs;

    // Grab the p values from the dataframe
    Rcpp::DoubleVector pValuesR = dataFrame["p_val"];
    QVector<double> pValues;

    // Grab the average log2 fold changes from the dataframe
    Rcpp::DoubleVector averageLog2FoldChangeR = dataFrame["avg_log2FC"];
    QVector<double> averageLog2FoldChange;

    // Grab the adjusted p values from the dataframe
    Rcpp::DoubleVector pValueAdjustedR = dataFrame["p_val_adj"];
    QVector<double> pValueAdjusted;

    // Transform all R-vectors into QVectors
    // The size of the pValuesR vector can be user here because in a
    // R dataframe each column must be of the same size by default
    for (int i = 0; i < pValuesR.size(); i++) {

        // The Rcpp string needs to be converted to a cpp-style string
        Rcpp::String geneIDString = geneIDsR[i];
        geneIDs.append(geneIDString.get_cstring());

        // The other values can be transferred as they are
        pValues.append(pValuesR[i]);
        averageLog2FoldChange.append(averageLog2FoldChangeR[i]);
        pValueAdjusted.append(pValueAdjustedR[i]);
    }

    qDebug() << geneIDs;
    qDebug() << pValues;

    exit(0);

//    QString samplesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/Pbmc_expression.csv";
//    QString cellTypesFilePath = "/home/numelen/Nextcloud/Documents/Arbeit/Hiwi/Daten/10xGenomics/PanglaoDB_markers.csv";

//    QString samplesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\10xGenomics\\Pbmc_expression.csv";
//    QString cellTypesFilePath = "C:\\Users\\Kademuni\\Nextcloud\\Documents\\Arbeit\\Hiwi\\Daten\\10xGenomics\\PanglaoDB_markers.csv";

//    qDebug() << "Parsing.";
//    QVector<FeatureCollection> samples = CSVReader::read10xGenomicsClustersFromFile(samplesFilePath, {15, 0});
//    QVector<FeatureCollection> cellTypes = CSVReader::readCellTypesFromPanglaoDBFile(cellTypesFilePath, {});
//    qDebug() << "Finished.";

//    samples.removeFirst();

//    QStringList completeGeneIDs;

//    for (FeatureCollection collection : samples) {
//        for (Feature feature : collection.getFeatures()) {
//            completeGeneIDs.append(feature.ID);
//        }
//    }

//    completeGeneIDs.removeDuplicates();

# else
    // Declaration of the used widgets
    MainWindow mainWindow;
    StartDialog startDialog;

    // Start of the main software - The start dialog is the first that is shown to the user
    startDialog.show();

    // ++++++++++++++++++++++++++++++++++++++++  CHECK FOR CONFIG FILE ++++++++++++++++++++++++++++++++++++++++
    QString configFilePath = QDir::homePath().append("/.badger.conf");
    ConfigFile configFile;

    bool isConfigFileExits = ConfigFileOperator::isConfigFileExists(configFilePath);

    if (isConfigFileExits) {
        configFile = ConfigFileOperator::readConfigFile(configFilePath);
        bool isDefaultMarkerFileExists = ConfigFileOperator::isFileExists(configFile.cellMarkersFilePath);

        if (!isDefaultMarkerFileExists) {
            startDialog.disableUseDefaultButton();
        }

    } else {
        ConfigFileOperator::createConfigFile(configFilePath);
        configFile = ConfigFileOperator::initializeConfigFile();
    }

    // ++++++++++++++++++++++++++++++++++++++++  CREATE PROGRAM BASICS  ++++++++++++++++++++++++++++++++++++++++

    // InformationCenter is used to store all relevant software data
    InformationCenter informationCenter(configFile);
    // Coordinator is used to control the main data flow
    Coordinator coordinator(informationCenter);


    // +++++++++++++++++++++++++++++++++++++  BUILD SIGNAL AND SLOT LOGIC  +++++++++++++++++++++++++++++++++++++
    // StartDialog -> Coordinator
    QObject::connect(&startDialog, &StartDialog::runNewProject, &coordinator, &Coordinator::on_newProjectStarted);

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedFileParsing, &mainWindow, &MainWindow::on_filesParsed);
    QObject::connect(&coordinator, &Coordinator::finishedCorrelating, &mainWindow, &MainWindow::on_correlatingFinished);
    QObject::connect(&coordinator, &Coordinator::sendGeneExpressionData, &mainWindow, &MainWindow::transmitGeneExpressionDataForAnalysisTab);
    QObject::connect(&coordinator, &Coordinator::sendDEAnalysisData, &mainWindow, &MainWindow::transmitDEAnalysisData);

    // Main Window -> Coordinator
    QObject::connect(&mainWindow, &MainWindow::runAnalysis, &coordinator, &Coordinator::on_runAnalysis);
    QObject::connect(&mainWindow, &MainWindow::requestGeneExpressionDataForAnalysisTab, &coordinator, &Coordinator::on_geneExpressionDataRequested);
    QObject::connect(&mainWindow, &MainWindow::requestDEAnalysis, &coordinator, &Coordinator::on_runDEAnalysis);

    // At this point, the complete control over the system workflow is handed over to the Coordinator

#endif
    return application.exec();
}
