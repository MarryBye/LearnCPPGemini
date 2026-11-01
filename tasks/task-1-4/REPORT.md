В ходе работы все собралось и файлы подключились. Для имопртирования мы создаем интерфейсный файл .hpp, его импортируем в файл реализации .cpp. Далее при сборке препроцессор тупо вставит интерфейсы в .cpp файл,а. линкер соединит функции с одинаковыми манглед именами. 

Чтобы не отдавать исходный код, но дать доступ к использованию библиотеки в будущем достаточно отдать скомпилированный бинарный файл + .hpp с интерфейсом. Конечный получатель подключает интерфейс, кладет рядом скомпилированный бинарник и сможет использовать наши технологии. Толкьо не до конца понимаю как ему подключить уже собранный бинарник?

Если убрать прагму из .hpp файла, подключающегося несколько раз, то будет ошибка

marrybye@MacBook-Air-Viktor task-1-4 % clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp sequence_math.cpp stats_printer.cpp -o app
In file included from main.cpp:2:
In file included from ./stats_printer.hpp:3:
./stats_types.hpp:3:8: error: redefinition of 'SequenceStats'
    3 | struct SequenceStats {
      |        ^
./sequence_math.hpp:3:10: note: './stats_types.hpp' included multiple times, additional include site here
    3 | #include "stats_types.hpp"
      |          ^
./stats_printer.hpp:3:10: note: './stats_types.hpp' included multiple times, additional include site here
    3 | #include "stats_types.hpp"
      |          ^
./stats_types.hpp:3:8: note: unguarded header; consider using #ifdef guards or #pragma once
    3 | struct SequenceStats {
      |        ^
1 error generated.

О переиспользовании, нету гардов либо прагмы.

Препроцессор вставляет много раз одно и то же и падает из-за переопределения одинаковых сигнатур. Если бы даже дальше пошло, чего не будет, дальше линкер не понимает с чем связывать сигнатуру, ведь реализация одна, а объявлений много выходит.