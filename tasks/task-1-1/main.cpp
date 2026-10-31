#include <iostream>

int multiply(int a, int b);
int calculate_power(int base, int exp);

int main() {
    std::cout << multiply(6, 7) << std::endl;
    std::cout << calculate_power(2, 10) << std::endl;

    return 0;
}