# Урок 1.6: Продвинутый базовый CMake — Режимы сборки (`Debug` vs `Release`), уровни оптимизации и индустриальная структура папок (`include/`, `src/`, `tests/`)

---

## 1. Тема и цель урока

В `task-1-5` мы создали наш первый `CMakeLists.txt`, но в нем осталась одна архитектурная проблема: мы **жестко прибили гвоздями** флаги `-g -O0 -fsanitize=address,undefined` прямо в `target_compile_options`!

Подумай, что произойдет, если мы захотим выкатить эту программу в продакшен для пользователей:
- В продакшене она будет работать **без оптимизаций (`-O0`)** и с замедлением от санитайзеров в 2–3 раза!
- А еще все файлы (`.hpp`, `.cpp`, `CMakeLists.txt`) пока лежат в одной плоской папке.

В этом уроке мы научимся собирать один и тот же проект в разных **режимах сборки (`CMAKE_BUILD_TYPE`)** и организуем настоящую взрослую структуру папок (`include/`, `src/`, `tests/`), где сразу увидим на практике ответ на твой вопрос из `task-1-5`: **зачем выделять код в библиотеку `add_library`, чтобы подключать её одновременно и к `app`, и к `tests`!**

---

## 2. Как это работает под капотом

### Четыре стандартных режима сборки в CMake (`CMAKE_BUILD_TYPE`)

В мире C++ один и тот же исходный код собирают в совершенно разных конфигурациях в зависимости от цели:

| Режим (`-DCMAKE_BUILD_TYPE=...`) | Какие флаги CMake подставляет сам | Для чего используется |
| :--- | :--- | :--- |
| **`Debug`** | `-g -O0` | Разработка и поиск багов. Оптимизации выключены, переменные видны в `lldb`, включаются санитайзеры и `assert()`. |
| **`Release`** | `-O3 -DNDEBUG` | Финальная сборка для продакшена. Максимальная скорость, векторизация циклов, отладочные символы и проверки `assert` полностью вырезаны (`-DNDEBUG`). |
| **`RelWithDebInfo`** | `-O2 -g -DNDEBUG` | **Любимый режим серверных и игровых разработчиков!** Код быстрый (`-O2`), но отладочные символы (`-g`) сохранены — если сервер упадет под нагрузкой, можно прочитать стектрейс и профилировать CPU! |
| **`MinSizeRel`** | `-Os -DNDEBUG` | Минимальный размер бинарника (для микроконтроллеров, мобилок и WebAssembly). |

### Уровни оптимизации компилятора (`-O0`, `-O1`, `-O2`, `-O3`)
Что именно делает компилятор при повышении уровня оптимизации?
- **`-O0`**: Компилирует строчку за строчкой буквально. Быстрая компиляция, медленный бинарник, идеальная отладка.
- **`-O1`**: Базовые оптимизации (хранит переменные в регистрах процессора вместо постоянной записи в ОЗУ на стек).
- **`-O2`**: Стандарт индустрии для релиза. Включает **Инлайнинг функций (Function Inlining — подстановка тела мелких функций прямо в место вызова без прыжка `bl`)**, вычисление констант на этапе компиляции, удаление мертвого кода и базовую векторизацию.
- **`-O3`**: Агрессивная оптимизация. Разворачивает большие циклы, активно использует SIMD-регистры (помнишь 16 параллельных умножений в `math_utils_optimized.s`?), иногда ценой увеличения размера бинарника.

---

### Как в CMake включать санитайзеры ТОЛЬКО в режиме `Debug`?

Вместо того чтобы зашивать `-g -O0` руками, мы позволяем самому CMake управлять флагами `-g` и `-O3` через `-DCMAKE_BUILD_TYPE=Debug` или `Release`, а санитайзеры подключаем по условию (или через опцию):

```cmake
# Если пользователь не передал -DCMAKE_BUILD_TYPE=..., по умолчанию ставим Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Базовые строгие предупреждения нужны ВСЕГДА (и в Debug, и в Release!)
target_compile_options(my_lib PUBLIC
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

# А санитайзеры включаем ТОЛЬКО в режиме Debug!
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(my_lib PUBLIC -fsanitize=address,undefined)
    target_link_options(my_lib PUBLIC -fsanitize=address,undefined)
endif()
```

Смотри, как это удобно в терминале — мы можем держать **две разные папки сборки одновременно**:
```bash
# Папка для разработки и отладки (с санитайзерами и -g -O0):
cmake -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug

# Папка для продакшена и замеров максимальной скорости (-O3 без санитайзеров):
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```
*(И в нашем `.gitignore` уже прописана маска `build-*/`, так что обе папки будут автоматически игнорироваться Git'ом!)*

---

### Индустриальная структура папок C++ проекта (`include/`, `src/`, `tests/`)

Начиная с этого урока, мы переходим от плоской свалки файлов к канонической структуре, принятой в 99% профессиональных C++ репозиториев:

```text
task-1-6/
├── CMakeLists.txt              <-- Конфигурация сборки
├── include/                    <-- Публичные заголовочные файлы (.hpp) библиотеки
│   ├── stats_types.hpp
│   ├── sequence_math.hpp
│   └── stats_printer.hpp
├── src/                        <-- Файлы реализации (.cpp) и точка входа приложения
│   ├── sequence_math.cpp
│   ├── stats_printer.cpp
│   └── main.cpp
└── tests/                      <-- Автоматические тесты (второй исполняемый таргет!)
    └── test_main.cpp
```

Почему именно так?
1. Когда мы пишем `target_include_directories(sequence_stats_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)`, все файлы внутри `src/` и `tests/` получают доступ к заголовкам из `include/` напрямую по имени (`#include "sequence_math.hpp"`), без всяких `../include/`.
2. Наша библиотека `sequence_stats_lib` собирается из `src/sequence_math.cpp` и `src/stats_printer.cpp`.
3. А затем мы создаем **два независимых исполняемых бинарника**:
   - `stats_app` (из `src/main.cpp`) — основная программа.
   - `stats_tests` (из `tests/test_main.cpp`) — программа, которая автоматически проверяет корректность всех наших функций!
   И оба бинарника просто линкуют одну и ту же `sequence_stats_lib`!

---

### Макрос `assert()` из `<cassert>` — простейший инструмент самопроверки

В Разделе 10 мы подключим полноценный тестовый фреймворк `GoogleTest`, но уже сейчас в стандартной библиотеке C++ есть заголовок **`<cassert>`** и макрос **`assert(условие)`**:
```cpp
#include <cassert>
#include "sequence_math.hpp"

int main() {
    assert(fibonacci(0) == 0);
    assert(fibonacci(1) == 1);
    assert(fibonacci(10) == 55);
    return 0;
}
```
**Как `assert` работает под капотом в `Debug` и `Release`?**
- В режиме **`Debug`** если условие внутри `assert(...)` оказывается ложным (`false`), программа мгновенно аварийно завершается (`abort()`) и пишет в консоль точный файл, номер строки и провалившееся условие:
  `Assertion failed: (fibonacci(10) == 55), function main, file test_main.cpp, line 7.`
- А в режиме **`Release`** CMake автоматически передает компилятору флаг **`-DNDEBUG`** (No Debug). И препроцессор **полностью стирает все `assert(...)` из кода**! В релизном бинарнике они занимают **0 байт и 0 тактов процессора**.

> **Важное правило безопасности с `assert()`**:
> Никогда не вызывай внутри `assert(...)` функции, которые меняют состояние программы (имеют побочные эффекты)! Иначе в `Debug` этот код будет выполняться, а в `Release` — бесследно исчезнет! Для чистых проверок в тестах лучше сохранить результат в переменную и проверить её, либо если тест должен работать и в `Release`, использовать обычную проверку `if (result != expected) return 1;`.

---

## 3. Особенности и подводные камни (Best Practices & Gotchas)

1. **После смены `CMAKE_BUILD_TYPE` нужно заново вызвать `cmake -B build ...` (или держать отдельные папки `build-debug` / `build-release`).**
2. **Не дублируй флаги на `executable`, если они уже `PUBLIC` на библиотеке.**
   В `task-1-5` ты увидел, что если у `sequence_stats_lib` стоит `target_compile_options(... PUBLIC ...)`, то любой таргет (`stats_app`, `stats_tests`), который сделает `target_link_libraries(... PRIVATE sequence_stats_lib)`, получит эти флаги, пути `include/` и санитайзеры **автоматически**!
3. **Встроенный тестовый раннер CMake (`enable_testing()` и `add_test()`):**
   Если в `CMakeLists.txt` добавить две строчки:
   ```cmake
   enable_testing()
   add_test(NAME SequenceUnitTests COMMAND stats_tests)
   ```
   То после сборки ты сможешь запускать все тесты проекта одной красивой командой **`ctest --test-dir build-debug --output-on-failure`** (точно как `pytest` в Python или `npm test` в TS!).

---

## 4. Примеры использования

### 🟢 Легкий (Easy): Проверка `CMAKE_BUILD_TYPE` в `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)
project(BuildTypesDemo LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Current build mode: ${CMAKE_BUILD_TYPE}")
```

---

### 🟡 Средний (Medium): Сборка библиотеки из `src/` и `include/`

```cmake
add_library(math_lib STATIC
    src/sequence_math.cpp
    src/stats_printer.cpp
)

# Указываем папку include/ как публичную витрину заголовков библиотеки
target_include_directories(math_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

---

### 🔴 Тяжелый (Hard): Полноценный `CMakeLists.txt` с двумя таргетами (`app` + `tests`), `CTest` и условными санитайзерами

```cmake
cmake_minimum_required(VERSION 3.20)
project(SequenceProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 1. Ядро-библиотека
add_library(sequence_stats_lib STATIC
    src/sequence_math.cpp
    src/stats_printer.cpp
)

target_include_directories(sequence_stats_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(sequence_stats_lib PUBLIC
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(sequence_stats_lib PUBLIC -fsanitize=address,undefined)
    target_link_options(sequence_stats_lib PUBLIC -fsanitize=address,undefined)
endif()

# 2. Основное приложение
add_executable(stats_app src/main.cpp)
target_link_libraries(stats_app PRIVATE sequence_stats_lib)

# 3. Тестовое приложение (линкует ту же самую библиотеку!)
add_executable(stats_tests tests/test_main.cpp)
target_link_libraries(stats_tests PRIVATE sequence_stats_lib)

# 4. Регистрируем тесты для запуска через ctest
enable_testing()
add_test(NAME AllSequenceTests COMMAND stats_tests)
```

---

## 5. Где это применяется в реальном мире

1. **Разделение `Debug` (для разработчиков и CI) и `Release` (для продакшена)**:
   В игровых движках (Unreal Engine, Godot) в режиме `Debug` игра может выдавать всего 20 FPS из-за тысяч проверок памяти и выключенной векторизации, а тот же самый код, собранный в `Release` (`-O3`), выдает 240 FPS!
2. **Архитектура «Библиотека ядра + Тонкий `main.cpp` + Тесты»**:
   Это золотой стандарт проектирования любого C++ сервиса. Именно поэтому ты выделял библиотеку в `task-1-5`, и сейчас ощутишь всю мощь этого решения на практике.

---

## 6. Практическое задание (`task-1-6`)

### Связь с предыдущими уроками
В `task-1-5` ты спросил: *«Зачем выделять `.cpp` файлы в отдельную библиотеку `sequence_stats_lib`?»*
Сейчас мы реорганизуем наш проект из `task-1-5` по взрослому стандарту (`include/`, `src/`, `tests/`) и соберем из одной библиотеки сразу **два исполняемых бинарника**: основную программу `stats_app` и автоматический тестовый раннер `stats_tests`, а также сравним `Debug` и `Release` сборки!

### Условие задачи
Создай директорию `tasks/task-1-6/` со следующей структурой подпапок:

```text
tasks/task-1-6/
├── CMakeLists.txt
├── REPORT.md
├── include/
│   ├── stats_types.hpp
│   ├── sequence_math.hpp
│   └── stats_printer.hpp
├── src/
│   ├── sequence_math.cpp
│   ├── stats_printer.cpp
│   └── main.cpp
└── tests/
    └── test_main.cpp
```

1. **Перенеси свои файлы из `task-1-5` по папкам `include/` и `src/`**:
   - В `src/stats_printer.cpp` поставь `#include "stats_printer.hpp"` на самую первую строчку (перед `#include <iostream>`).
   - В `src/sequence_math.cpp` в функции `fibonacci` объявляй переменную `long long c = a + b;` прямо внутри цикла `for` (как мы разобрали в ревью `1-5`).
2. **Напиши тестовый файл `tasks/task-1-6/tests/test_main.cpp`**:
   - Подключи `"sequence_math.hpp"` и `<iostream>`.
   - В функции `main()` напиши проверки для всех наших функций (`sum_of_squares`, `factorial`, `fibonacci`, `build_sequence_stats`), включая граничные случаи (`n = -5`, `n = 0`, `n = 1`, `n = 5`, `n = 10`).
   - Сделай проверки через обычный `if` (чтобы они работали и в `Debug`, и в `Release`!): если хотя бы один результат не совпал с ожидаемым — выводи сообщение об ошибке в `std::cerr` и возвращай `return 1;` (код ошибки для `ctest`). Если все проверки прошли — выводи `"All unit tests passed!\n"` и возвращай `return 0;`.
3. **Напиши `tasks/task-1-6/CMakeLists.txt`**:
   - Настрой `CMAKE_BUILD_TYPE` по умолчанию в `Debug` (если не задан).
   - Собери библиотеку `sequence_stats_lib` из `src/sequence_math.cpp` и `src/stats_printer.cpp`, указав ей `target_include_directories(... PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)`.
   - Навесь на `sequence_stats_lib` (`PUBLIC`) флаги `-Wall -Wextra -Wpedantic -Werror`, а флаги `-fsanitize=address,undefined` — **только если `CMAKE_BUILD_TYPE STREQUAL "Debug"`** (не дублируй эти флаги на `stats_app` и `stats_tests`, пусть они унаследуются автоматически!).
   - Создай два исполняемых таргета: `stats_app` (`src/main.cpp`) и `stats_tests` (`tests/test_main.cpp`), прилинковав к обоим `PRIVATE sequence_stats_lib`.
   - Подключи `enable_testing()` и `add_test(NAME AllSequenceTests COMMAND stats_tests)`.
4. **Проведи эксперимент для `tasks/task-1-6/REPORT.md`**:
   - Собери проект в двух разных папках:
     ```bash
     # 1. Debug сборка
     cmake -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
     cmake --build build-debug
     ctest --test-dir build-debug --output-on-failure

     # 2. Release сборка
     cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
     cmake --build build-release
     ctest --test-dir build-release --output-on-failure
     ```
   - Сравни размер скомпилированных бинарников `build-debug/stats_app` и `build-release/stats_app` через команду `ls -lh build-debug/stats_app build-release/stats_app`.
   - Загляни в файлы `build-debug/compile_commands.json` и `build-release/compile_commands.json` — какие именно флаги оптимизации и отладки CMake автоматически подставил в команду `clang++` для `Debug` и для `Release`?
   - Запиши результаты в `tasks/task-1-6/REPORT.md`.

Жду твой проект!
