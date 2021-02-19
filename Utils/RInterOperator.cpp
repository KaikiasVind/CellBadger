#include "RInterOperator.h"

#include <QStringList>
#include <QDebug>

RInterOperator::RInterOperator() {
    this->loadNecessaryLibraries();
}


/**
 * @brief RInterOperator::calculateDifferentialExpressedGenes - Calculate the differential expression of the genes from the given expression data
 * @param matrixFilePath - Path to the raw matrix data file path
 * @param clusteringInformationFilePath - Path to the file containing the gene-cluster mapping
 * @param clustersToCompare - A list of clusters that are supposed to be compared here.
 * @return - List of pairs representing the DE analysis matrix with the differentially expressed genes
 */
QVector<QPair<QString, QVector<double>>> RInterOperator::calculateDifferentialExpressedGenes(const QString matrixFilePath, const QString clusteringInformationFilePath, const QVector<int> clustersToCompare) {
    QVector<QPair<QString, QVector<double>>> differentiallyExpressedGenes;

    // Assign the given parameters to R variables
    this->rSession.assign(clusteringInformationFilePath.toStdString(), "clustering.path");
    this->rSession.assign(std::vector<int>(clustersToCompare.begin(), clustersToCompare.end()), "wanted.clusters");
    this->rSession.assign(matrixFilePath.toStdString() + "/barcodes.tsv.gz", "barcodes.path");
    this->rSession.assign(matrixFilePath.toStdString() + "/features.tsv.gz", "features.path");
    this->rSession.assign(matrixFilePath.toStdString() + "/matrix.mtx.gz", "matrix.path");

    // Parse clustering data
    this->rSession.parseEvalQ("clustering.data <- read.csv(clustering.path)");

    // Parse and create expression matrix
    this->rSession.parseEvalQ("matrix <- readMM(file = matrix.path)");
    this->rSession.parseEvalQ("feature.names = read.delim(features.path, header = FALSE, stringsAsFactors = FALSE)");
    this->rSession.parseEvalQ("barcode.names = read.delim(barcodes.path, header = FALSE, stringsAsFactors = FALSE)");
    this->rSession.parseEvalQ("colnames(matrix) = barcode.names$V1");
    this->rSession.parseEvalQ("rownames(matrix) = feature.names$V1");

    // Create and prepare a Seurat object from the given data of the expression matrix
    this->rSession.parseEvalQ("seurat <- CreateSeuratObject(counts = matrix, min.cells = 1, min.features = 1)");
    this->rSession.parseEvalQ("idx <- match(Cells(seurat), clustering.data$Barcode)");
    this->rSession.parseEvalQ("seurat$clustering <- clustering.data$Cluster[idx]");
    this->rSession.parseEvalQ("Idents(seurat) <- 'clustering'");

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
    this->rSession.parseEvalQ("DEG <- " + findMarkersEvalString.toStdString());

    // Grab the data frame that was created by the DE analysis
    Rcpp::DataFrame dataFrame = this->rSession.parseEval("DEG");

    // Grab the gene ids from the dataframe's row name's
    Rcpp::StringVector geneIDsR = this->rSession.parseEval("rownames(DEG)");

    // Grab the p values from the dataframe
    Rcpp::DoubleVector pValuesR = dataFrame["p_val"];

    // Grab the average log2 fold changes from the dataframe
    Rcpp::DoubleVector averageLog2FoldChangeR = dataFrame["avg_log2FC"];

    // Grab the adjusted p values from the dataframe
    Rcpp::DoubleVector pValueAdjustedR = dataFrame["p_val_adj"];

    // Go through all R-vectors to grab the data
    // The size of the pValuesR vector can be user here because in a
    // R dataframe each column must be of the same size by default
    for (int i = 0; i < pValuesR.size(); i++) {

        // The Rcpp string needs to be cast to a cpp-style string before usage
        QString geneID = static_cast<Rcpp::String>(geneIDsR[i]).get_cstring();

        // The other values can be uses as they are
        QVector<double> analysisValues = {pValuesR[i], averageLog2FoldChangeR[i], pValueAdjustedR[i]};

        // Apend the newly assembled row to the data block
        differentiallyExpressedGenes.append(qMakePair(geneID, analysisValues));
    }

    // Return all gathered data
    // Each pair stands for one data frame row, i.e. one gene with corresponding analysis values
    return differentiallyExpressedGenes;
}


/**
 * @brief RInterOperator::loadNecessaryLibraries - Load the libraries necessary for the correct execution
 */
void RInterOperator::loadNecessaryLibraries() {
    this->rSession.parseEvalQ("library(Seurat)");
    this->rSession.parseEvalQ("library(Matrix)");
}

