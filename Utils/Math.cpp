#include "Math.h"

#include <math.h>

namespace Math {

template <typename N, typename R>
/**
 * @brief mean
 * @param numbers
 * @return
 */
R mean(QVector<N> numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), .0) / numbers.length();
}

/**
 * @brief mean
 * @param numbers
 * @return
 */
double mean(QVector<double> numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), .0) / numbers.length();
}

/**
 * @brief mean
 * @param numbers
 * @return
 */
double mean(QVector<int> numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), .0) / numbers.length();
}

/**
 * @brief invertLog - Takes a number x and a base number b with log_b(y) = x and returns y (with b € N, x,y € R)
 * @param base - Base for log function
 * @param realNumber - Number x that was the result of log_b(y) = x
 * @return - Inverts the log function and presents y = b^x
 */
double invertLog(int base, double realNumber) {
    return pow(base, realNumber);
}

}


