#include <iostream>
#include "stats_printer.hpp"

void print_stats(const SequenceStats& stats) {
    std::cout << "Count: " << stats.count << '\n';
    std::cout << "Sum of squares: " << stats.sum_of_squares << '\n';
    std::cout << "Factorial: " << stats.factorial << '\n';
    std::cout << "Fibonacci: " << stats.fibonacci << '\n';
}