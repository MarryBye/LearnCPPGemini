// Функция 1: Возведение в степень (из прошлого урока)
long long safe_power(int base, int exp) {
    if (exp < 0) return 0;
    if (exp == 0) return 1;
    long long result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
} // Баг 1: А где return?!

// Функция 2: Получение элемента из фиксированного буфера метрик
int get_metric_by_index(int index) { // Баг 2: неиспользуемый параметр
    int metrics[4] = {10, 20, 30, 40};
    if (index < 0 || index >= 4) {
        return -1; // Возвращаем -1 при выходе за границы массива
    }
    return metrics[index]; // Баг 3: Что если index < 0 или index >= 4?
}
