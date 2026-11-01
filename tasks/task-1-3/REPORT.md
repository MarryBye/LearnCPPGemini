1. Собрал файлы в самом начале без измененй и дебага

marrybye@MacBook-Air-Viktor task-1-3 % clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp sequence.cpp -o app 
marrybye@MacBook-Air-Viktor task-1-3 % 

-g - создалась с папка с дебаг символами, чтобы потом при дебаги видеть настоящие названия переменных
-O0 - юез оптимизации, чтобы переменные не стерлись компилятором 

Результат программы: 
Sum of squares (4): 9
Factorial (5): 0

2. Брейкпоинт на сумму квадратор

       frame #0: 0x0000000100003788 app`sum_of_squares(n=4) at sequence.cpp:4:9
   1    // Функция 1: Считает сумму квадратов чисел от 1 до n включительно:
   2    // Например, для n = 3 должно быть: 1^2 + 2^2 + 3^2 = 1 + 4 + 9 = 14.
   3    long long sum_of_squares(int n) {
-> 4        if (n <= 0) {
   5            return 0;
   6        }
   7        long long total = 0;
Target 6: (app) stopped.

(int) n = 4
(long long) total = 8303480224

* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 4.1
   * frame #0: 0x0000000100003788 app`sum_of_squares(n=4) at sequence.cpp:4:9
     frame #1: 0x00000001000008d0 app`main at main.cpp:7:20
     frame #2: 0x000000018af97e80 dyld`start + 6688

n=4, но сработало n <= 0?

ВОт потом во время обката программы работало вот так

Target 6: (app) stopped.
(lldb) fr v
(int) n = 4
(long long) total = 0
(int) i = 1
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000037c8 app`sum_of_squares(n=4) at sequence.cpp:9:17
   6        }
   7        long long total = 0;
   8        for (int i = 1; i < n; ++i) {
-> 9            total = i * i;
   10       }
   11       return total;
   12   }
Target 6: (app) stopped.
(lldb) fr v
(int) n = 4
(long long) total = 0
(int) i = 1
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x000000010000382c app`sum_of_squares(n=4) at sequence.cpp:10:5
   7        long long total = 0;
   8        for (int i = 1; i < n; ++i) {
   9            total = i * i;
-> 10       }
   11       return total;
   12   }
   13  
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x0000000100003830 app`sum_of_squares(n=4) at sequence.cpp:8:28
   5            return 0;
   6        }
   7        long long total = 0;
-> 8        for (int i = 1; i < n; ++i) {
   9            total = i * i;
   10       }
   11       return total;
Target 6: (app) stopped.
(lldb) fr v
(int) n = 4
(long long) total = 1
(int) i = 1
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000037c8 app`sum_of_squares(n=4) at sequence.cpp:9:17
   6        }
   7        long long total = 0;
   8        for (int i = 1; i < n; ++i) {
-> 9            total = i * i;
   10       }
   11       return total;
   12   }
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x000000010000382c app`sum_of_squares(n=4) at sequence.cpp:10:5
   7        long long total = 0;
   8        for (int i = 1; i < n; ++i) {
   9            total = i * i;
-> 10       }
   11       return total;
   12   }
   13  
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x0000000100003830 app`sum_of_squares(n=4) at sequence.cpp:8:28
   5            return 0;
   6        }
   7        long long total = 0;
-> 8        for (int i = 1; i < n; ++i) {
   9            total = i * i;
   10       }
   11       return total;
Target 6: (app) stopped.
(lldb) fr v
(int) n = 4
(long long) total = 4
(int) i = 2
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000037c8 app`sum_of_squares(n=4) at sequence.cpp:9:17
   6        }
   7        long long total = 0;
   8        for (int i = 1; i < n; ++i) {
-> 9            total = i * i;
   10       }
   11       return total;
   12   }
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x000000010000382c app`sum_of_squares(n=4) at sequence.cpp:10:5
   7        long long total = 0;
   8        for (int i = 1; i < n; ++i) {
   9            total = i * i;
-> 10       }
   11       return total;
   12   }
   13  
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x0000000100003830 app`sum_of_squares(n=4) at sequence.cpp:8:28
   5            return 0;
   6        }
   7        long long total = 0;
-> 8        for (int i = 1; i < n; ++i) {
   9            total = i * i;
   10       }
   11       return total;
Target 6: (app) stopped.
(lldb) fr v
(int) n = 4
(long long) total = 9
(int) i = 3
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x0000000100003870 app`sum_of_squares(n=4) at sequence.cpp:11:12
   8        for (int i = 1; i < n; ++i) {
   9            total = i * i;
   10       }
-> 11       return total;
   12   }
   13  
   14   // Функция 2: Считает факториал числа n (1 * 2 * ... * n), при n <= 1 возвращает 1.
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x000000010000387c app`sum_of_squares(n=4) at sequence.cpp:12:1
   9            total = i * i;
   10       }
   11       return total;
-> 12   }
   13  
   14   // Функция 2: Считает факториал числа n (1 * 2 * ... * n), при n <= 1 возвращает 1.
   15   // Например, для n = 5 должно быть: 120.
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000008d0 app`main at main.cpp:7:10
   4    long long factorial(int n);
   5   
   6    int main() {
-> 7        auto squares = sum_of_squares(4); // Ожидаем: 1 + 4 + 9 + 16 = 30
   8        auto fact = factorial(5);         // Ожидаем: 120
   9   
   10       std::cout << "Sum of squares (4): " << squares << '\n';
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000008d4 app`main at main.cpp:8:17
   5   
   6    int main() {
   7        auto squares = sum_of_squares(4); // Ожидаем: 1 + 4 + 9 + 16 = 30
-> 8        auto fact = factorial(5);         // Ожидаем: 120
   9   
   10       std::cout << "Sum of squares (4): " << squares << '\n';
   11       std::cout << "Factorial (5): " << fact << '\n';
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 5.1
       frame #0: 0x000000010000389c app`factorial(n=5) at sequence.cpp:17:9
   14   // Функция 2: Считает факториал числа n (1 * 2 * ... * n), при n <= 1 возвращает 1.
   15   // Например, для n = 5 должно быть: 120.
   16   long long factorial(int n) {
-> 17       if (n <= 1) {
   18           return 1;
   19       }
   20       long long result = 0;
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000038b8 app`factorial(n=5) at sequence.cpp:20:15
   17       if (n <= 1) {
   18           return 1;
   19       }
-> 20       long long result = 0;
   21       for (int i = 2; i <= n; ++i) {
   22           result *= i;
   23       }
Target 6: (app) stopped.
(lldb) fr v
(int) n = 5
(long long) result = 9
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000038bc app`factorial(n=5) at sequence.cpp:21:14
   18           return 1;
   19       }
   20       long long result = 0;
-> 21       for (int i = 2; i <= n; ++i) {
   22           result *= i;
   23       }
   24       return result;
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000038e0 app`factorial(n=5) at sequence.cpp:22:19
   19       }
   20       long long result = 0;
   21       for (int i = 2; i <= n; ++i) {
-> 22           result *= i;
   23       }
   24       return result;
   25   }
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x000000010000392c app`factorial(n=5) at sequence.cpp:23:5
   20       long long result = 0;
   21       for (int i = 2; i <= n; ++i) {
   22           result *= i;
-> 23       }
   24       return result;
   25   }
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x0000000100003930 app`factorial(n=5) at sequence.cpp:21:29
   18           return 1;
   19       }
   20       long long result = 0;
-> 21       for (int i = 2; i <= n; ++i) {
   22           result *= i;
   23       }
   24       return result;
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000038e0 app`factorial(n=5) at sequence.cpp:22:19
   19       }
   20       long long result = 0;
   21       for (int i = 2; i <= n; ++i) {
-> 22           result *= i;
   23       }
   24       return result;
   25   }
Target 6: (app) stopped.
(lldb) n
Process 25956 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x000000010000392c app`factorial(n=5) at sequence.cpp:23:5
   20       long long result = 0;
   21       for (int i = 2; i <= n; ++i) {
   22           result *= i;
-> 23       }
   24       return result;
   25   }
Target 6: (app) stopped.
(lldb) p &total
          ˄
          ╰─ error: use of undeclared identifier 'total'
note: Ran expression as 'C++14'.
(lldb) p &result
(long long *) 0x000000016fdfe8e8

4. Ошибки

sum_of_squares

1. n=4, total=3803480224
2. n=4, total=0, i=1
3. n=4, total=4, i=2
4. n=4, total=9, i=3

Не было 5-го прохода цикла для i=4. Исправить можно сделать <= n. Также напомни разщницу между ++i & i++

В переменную тотал не  прибавляются значения, а просто приравнивается

factorial

1. n=5, result=0, i=4
2. n=5, result=0, i=2
3. n=5, result=0, i=3
4. ...

Переменная result = 0, а при умножении она всегда будет 0 соответственно. result должно быть =1, либо надо прибавлять результат умножения. 

Sum of squares (4): 30
Factorial (5): 120