#ifndef MATH_H
#define MATH_H

#include <QVector>

namespace Math {

    template <typename N, typename R>
    extern R mean(QVector<N> numbers);

    // REMEMBER: Exchange type specific mean functions with template
    extern double mean(QVector<double> numbers);
    extern double mean(QVector<int> numbers);
}

#endif // MATH_H
