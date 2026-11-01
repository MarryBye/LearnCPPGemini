#include "sequence_math.hpp"

long long sum_of_squares(int n) {
    if (n <= 0) {
        return 0;
    }
    long long total = 0;
    for (int i = 1; i <= n; ++i) {
        total += i * i;
    }
    return total;
}

long long factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

SequenceStats build_sequence_stats(int n) {
    SequenceStats stats;
    stats.count = (n > 0) ? n : 0;
    stats.sum_of_squares = sum_of_squares(n);
    stats.factorial = factorial(n);
    return stats;
}