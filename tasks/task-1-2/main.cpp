#include <iostream>

long long safe_power(int base, int exp);
int get_metric_by_index(int index);

int main() {

    auto first = get_metric_by_index(4);
    auto second = safe_power(2, 31);

    // Вызов 1: выход за границы массива (индекс 4 при размере 4)
    std::cout << "Metric: " << first << '\n';

    // Вызов 2: переполнение знакового int (2^31 не влезает в знаковый 32-битный int!)
    std::cout << "Power: " << second << '\n';
    return 0;
}
