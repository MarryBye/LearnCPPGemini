1. При выполнении

marrybye@MacBook-Air-Viktor task-1-2 % clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp analytics.cpp -o app

analytics.cpp:5:9: error: variable 'result' set but not used [-Werror,-Wunused-but-set-variable]
    5 |     int result = 1;
      |         ^
analytics.cpp:9:1: error: non-void function does not return a value in all control paths [-Werror,-Wreturn-type]
    9 | } // Баг 1: А где return?!
      | ^
analytics.cpp:12:40: error: unused parameter 'bonus_unused' [-Werror,-Wunused-parameter]
   12 | int get_metric_by_index(int index, int bonus_unused) { // Баг 2: неиспользуемый параметр
      |                                        ^
3 errors generated.
marrybye@MacBook-Air-Vi

Когда намеренно исправлял ошибки, сначала забыл убрать неиспользуемый параметр, потом линкер не понял что делать из-за того, что не одинаковый мангл имена были (в мейне сигнатура была с параметром, в аналитике без). В итоге исправил, билдится

marrybye@MacBook-Air-Viktor task-1-1 % clang++ main.o math_utils.o -o app
marrybye@MacBook-Air-Viktor task-1-1 % cd ../task-1-2
marrybye@MacBook-Air-Viktor task-1-2 % clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp analytics.cpp -o app

analytics.cpp:5:9: error: variable 'result' set but not used [-Werror,-Wunused-but-set-variable]
    5 |     int result = 1;
      |         ^
analytics.cpp:9:1: error: non-void function does not return a value in all control paths [-Werror,-Wreturn-type]
    9 | } // Баг 1: А где return?!
      | ^
analytics.cpp:12:40: error: unused parameter 'bonus_unused' [-Werror,-Wunused-parameter]
   12 | int get_metric_by_index(int index, int bonus_unused) { // Баг 2: неиспользуемый параметр
      |                                        ^
3 errors generated.
marrybye@MacBook-Air-Viktor task-1-2 % clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp analytics.cpp -o app

Undefined symbols for architecture arm64:
  "get_metric_by_index(int, int)", referenced from:
      _main in main-25661e.o
ld: symbol(s) not found for architecture arm64
clang++: error: linker command failed with exit code 1 (use -v to see invocation)
marrybye@MacBook-Air-Viktor task-1-2 % clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp analytics.cpp -o app

main.cpp:8:32: error: no matching function for call to 'get_metric_by_index'
    8 |     std::cout << "Metric: " << get_metric_by_index(4, 999) << '\n';
      |                                ^~~~~~~~~~~~~~~~~~~
main.cpp:4:5: note: candidate function not viable: requires single argument 'index', but 2 arguments were provided
    4 | int get_metric_by_index(int index);
      |     ^                   ~~~~~~~~~
1 error generated.
marrybye@MacBook-Air-Viktor task-1-2 % clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp analytics.cpp -o app

3. При выполнении программы падает

analytics.cpp:7:16: runtime error: signed integer overflow: 1073741824 * 2 cannot be represented in type 'int'
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior analytics.cpp:7:16 
Power: -2147483648

Saving session...
...copying shared history...
...saving history...truncating history files...
...completed.

[Process completed]

Переполнение 32 битного интеджера 2 в 31 степени

Исправленой сменой типа данных на long long

ВОПРОС

В чем разница между

int
long int
long long int
long long

Все 4 варианта не дают ошибку, но хочу понять принциипальную разницу типов данных и вообще что делает само слово long