#include "Math.h"

namespace Math {

template <typename N, typename R>
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

}
