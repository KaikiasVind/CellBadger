#include "Correlator.h"

#include <QVector>
#include <QDebug>
#include <math.h>

#include "Utils/Math.h"

namespace Correlator {

/**
 * @brief calculateSpearmanCorrelation - Calculates the spearman correlation coefficient for two given variables.
 * @param variableOne - Vector of numbers representing the attributes of the first variable
 * @param variableTwo - Vector of numbers representing the attributes of the first variable
 * @return - Correlation coefficient in range [0,1] with 1 = full correlation.
 */
double calculateSpearmanCorrelation(QVector<double> variableOne, QVector<double> variableTwo) {

    // What to do if the length is not equal?
    if (variableOne.length() != variableTwo.length()) {
        qDebug() << "Variables do not have the same length.";
        exit(0);
    }

    auto getRanks = [](QVector<double> collection) {
        // Sort numbers to calculate ranks
        QVector<double> sortedCollection = collection;
        std::sort(sortedCollection.begin(), sortedCollection.end());

        QVector<int> ranks;
        for (double number : collection) {
            ranks.append(sortedCollection.indexOf(number));
        }
        return ranks;
    };

    QVector<int> variableOneRanks = getRanks(variableOne),
                 variableTwoRanks = getRanks(variableTwo);

    // Counter and denominator are calculated independently and divided later on
    double counter = .0, denominator = .0;

    // Get ranks (indizes) of the expression counts (+1 because index starts at 0)

    // Calculate the mean for the expression count ranks
    double variableOneRankMean = Math::mean(variableOneRanks),
           variableTwoRankMean = Math::mean(variableTwoRanks);

    double denominatorFactorOne = .0, denominatorFactorTwo = .0;

    // This loop serves all of the three sums in the spearman-correlation formula.
    // It would be easier to read to use three seperate loops, but as the three sums have the
    // same range they can be represented with one loop to reduce calculation length
    for (int i = 0; i < variableOne.length(); i++) {
        int rankOfVarOne = variableOneRanks[i],
            rankOfVarTwo = variableTwoRanks[i];

        // Calculate counter
        double counterFactorOne = rankOfVarOne - variableOneRankMean,
               counterFactorTwo = rankOfVarTwo - variableTwoRankMean;
        counter += counterFactorOne * counterFactorTwo;

        // Calculate the two factors in the denominator
        // The sqrt of the factors is needed here, so they are added up here first and sqrted later
        denominatorFactorOne += pow(rankOfVarOne - variableOneRankMean, 2);
        denominatorFactorTwo += pow(rankOfVarTwo - variableTwoRankMean, 2);
    }

    // Multiply the two factors of the denominator that were calculated in the loop
    denominator = sqrt(denominatorFactorOne) * sqrt(denominatorFactorTwo);

    // Calculating the final value for r_Sp
    double result = counter / denominator;

    return result;
}

}
