#ifndef CORRELATIONFINDER_H
#define CORRELATIONFINDER_H

#include <QVector>

namespace Correlator
{
    extern double calculateSpearmanCorrelation(QVector<double> variableOne, QVector<double> variableTwo);
    extern double calculatePearsonCorrelation(QVector<double> variableOne, QVector<double> variableTwo);
};

#endif // CORRELATIONFINDER_H
