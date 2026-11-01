#include "sequence_math.hpp"
#include "stats_printer.hpp"

int main() {
    int n = 5;
    SequenceStats stats = build_sequence_stats(n);
    print_stats(stats);
    return 0;
}