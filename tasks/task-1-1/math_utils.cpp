int multiply(int a, int b) {
    return a * b;
}

int calculate_power(int base, int exp) {
    if (exp <= 0) return 1;

    int result = 1;

    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    
    return result;
}