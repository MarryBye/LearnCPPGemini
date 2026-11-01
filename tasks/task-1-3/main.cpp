#include <iostream>

long long sum_of_squares(int n);
long long factorial(int n);

int main() {
    auto squares = sum_of_squares(4); // Ожидаем: 1 + 4 + 9 + 16 = 30
    auto fact = factorial(5);         // Ожидаем: 120

    std::cout << "Sum of squares (4): " << squares << '\n';
    std::cout << "Factorial (5): " << fact << '\n';
    return 0;
}
