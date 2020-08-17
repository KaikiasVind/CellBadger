#ifndef MATH_H
#define MATH_H

#include <QVector>

#include "BioModels/FeatureCollection.h"

namespace Math {

// ####################################### OPERATIONS #######################################
template <typename N, typename R>
extern R mean(QVector<N> numbers);
// REMEMBER: Exchange type specific mean functions with template
extern double mean(QVector<double> numbers);
extern double mean(QVector<int> numbers);
extern double invertLog(int base, double realNumber);

// ####################################### SPECIFICS #######################################
extern QVector<double> calculateQualityScores(QVector<QVector<QPair<QString, double>>> correlatedDataset);
extern double calculateMeanRawCountForGene(QString geneID, QVector<FeatureCollection> clusters);

}

#endif // MATH_H
