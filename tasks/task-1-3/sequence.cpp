// Функция 1: Считает сумму квадратов чисел от 1 до n включительно:
// Например, для n = 3 должно быть: 1^2 + 2^2 + 3^2 = 1 + 4 + 9 = 14.
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

// Функция 2: Считает факториал числа n (1 * 2 * ... * n), при n <= 1 возвращает 1.
// Например, для n = 5 должно быть: 120.
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
