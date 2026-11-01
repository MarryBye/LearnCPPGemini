# Урок 1.5: Введение в CMake — стандарт де-факто в индустрии C++

---

## 1. Тема и цель урока

В `task-1-4` у нас было всего три `.cpp` файла, а команда компиляции уже разрослась до двух строчек:
```bash
clang++ -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined main.cpp sequence_math.cpp stats_printer.cpp -o app
```

А теперь представь реальный проект из 200 файлов:
1. Писать команду на 200 файлов вручную невозможно.
2. Пересобирать все 200 файлов каждый раз, когда ты поменял одну строчку в `main.cpp` — безумие (мы помним из Урока 1.4 про инкрементальную сборку!).
3. Если твой коллега сидит на Windows с компилятором `MSVC` (`cl.exe`), твоя команда для `clang++` у него вообще не запустится, потому что в MSVC флаги называются иначе (`/W4 /WX /std:c++20`).

В TypeScript ты описываешь проект в `package.json` + `tsconfig.json`, в Python — в `pyproject.toml`. В мире C++ абсолютным стандартом индустрии (более 85% всех коммерческих и open-source проектов в мире) является **CMake** и его файл конфигурации **`CMakeLists.txt`**.

**Цель этого урока:**
- Понять, почему CMake — это не просто система сборки, а **мета-система сборки (генератор)**.
- Освоить **Modern CMake (таргет-ориентированный подход)**: `add_executable`, `add_library`, `target_link_libraries`, `target_compile_options`.
- Увидеть на практике ответ на твой вопрос из `task-1-4`: как CMake сам собирает код в файл библиотеки (`lib...a`) и подключает его к приложению одной строчкой.
- Научиться делать правильную **Out-of-source сборку (`build/`)** с быстрым генератором **Ninja**.

---

## 2. Как это работает под капотом

### Главный секрет: CMake сам НЕ компилирует код!

Многие новички думают, что CMake — это компилятор или замена `make`. Это не так!
**CMake — это генератор систем сборки (Meta-Build System).**

Почему он устроен именно так?
Потому что на разных платформах свои низкоуровневые инструменты сборки:
- На Linux и macOS исторически правит **`Make`** (`Makefile`) и сверхбыстрый современный сборщик **`Ninja`** (`build.ninja`).
- На Windows разработчики часто хотят открыть проект в Visual Studio (`.sln` / `.vcxproj`).
- На macOS кто-то хочет открыть проект в Xcode (`.xcodeproj`).

Ты пишешь **один универсальный файл `CMakeLists.txt`**, а работа с CMake всегда делится на **2 этапа**:

```
Этап 1: КОНФИГУРАЦИЯ И ГЕНЕРАЦИЯ (выполняется 1 раз или при добавлении новых файлов)
[ CMakeLists.txt ] ──( cmake -B build -G Ninja )──► [ папка build/ (внутри build.ninja) ]
                                                               │
Этап 2: СБОРКА (выполняется при каждом изменении кода)         ▼
                                             ( cmake --build build )
                                                               │
                                                               ▼
                                          Ninja вызывает clang++ только для
                                          изменившихся .cpp файлов и линкует app!
```

### Что такое Out-of-source сборка (`-B build`)?
В первых четырех уроках мы запускали `clang++` прямо в папке с исходниками, и рядом с `.cpp` появлялись мусорные `.o`, `.dSYM` и `app`.
В CMake так делать **строго запрещено**. Всегда используется **Out-of-source build**:
- Исходники лежат чистыми в папке задачи.
- Все объектные файлы `.o`, скомпилированные библиотеки `.a`, кэш и финальный бинарник складываются в изолированную папку **`build/`** (которая уже добавлена в наш `.gitignore`!).
- Хочешь полностью очистить проект с нуля? Просто удаляешь папку `rm -rf build` — и твои исходники девственно чисты!

---

### Философия Modern CMake: всё есть Таргет (Target)

В старом CMake (до 2015 года, который до сих пор встречается в плохих туториалах) настройки задавались глобально на всю папку.
В **Современном CMake (Modern CMake 3.15+)** мы мыслим **Таргетами (Целями сборки)** — точно так же, как объектами в ООП!

Существует два главных вида таргетов:
1. **`add_executable(app_name main.cpp ...)`** — создает исполняемый файл (программу с функцией `main()`).
2. **`add_library(lib_name STATIC math.cpp ...)`** — создает библиотеку (набор функций/классов **без** `main()`), которую можно подключать к другим таргетам!
   - На твоем Mac CMake автоматически превратит таргет `add_library(sequence_math STATIC ...)` в файл архива машинного кода **`libsequence_math.a`** внутри папки `build/`!

А как подключить эту библиотеку к исполняемому файлу `app_name`? Одной строчкой:
```cmake
target_link_libraries(app_name PRIVATE sequence_math)
```
Когда ты пишешь эту строчку, CMake делает всю грязную работу за тебя:
1. Сначала собирает библиотеку `libsequence_math.a`.
2. Передает путь к этой библиотеке и нужные флаги линкеру при сборке `app_name`.
3. Если у библиотеки были публичные папки с `.hpp` файлами или флаги компиляции, CMake **автоматически** передает их и в `app_name`!

---

### Модификаторы видимости в CMake: `PRIVATE`, `PUBLIC`, `INTERFACE`

Когда мы навешиваем свойства на таргет (например, подключаем к нему библиотеку или задаем флаги), CMake требует указать одно из трех ключевых слов. Их очень легко понять на примере библиотеки `MathLib` и приложения `App`, которое её использует:

| Ключевое слово | Для самой `MathLib` | Наследуется ли в `App` (кто подключил `MathLib`)? | Когда использовать? |
| :--- | :---: | :---: | :--- |
| **`PRIVATE`** | ✅ Да | ❌ Нет | Внутренняя деталь реализации (нужна только при компиляции `.cpp` самой `MathLib`). |
| **`PUBLIC`** | ✅ Да | ✅ Да | Часть интерфейса (нужна и самой `MathLib`, и всем, кто подключает её `.hpp` заголовки). |
| **`INTERFACE`** | ❌ Нет | ✅ Да | Нужно только тем, кто подключает библиотеку (например, библиотека состоит только из `.hpp` файлов без `.cpp`). |

---

## 3. Особенности и подводные камни (Best Practices & Gotchas)

1. **Никогда не используй `file(GLOB ...)` для поиска `.cpp` файлов в `CMakeLists.txt`!**
   Иногда новички ленятся перечислять файлы и пишут `file(GLOB SOURCES "*.cpp")`.
   **Почему это зло:** CMake сканирует папку только на Этапе 1 (`cmake -B build`). Если ты потом создашь новый файл `helper.cpp` и запустишь сборку `cmake --build build`, система сборки Ninja **не узнает**, что появился новый файл, и не включит его в сборку! Всегда явно перечисляй `.cpp` файлы в `add_executable` / `add_library`.
2. **Указывай только `.cpp` файлы в `add_executable` / `add_library` (заголовки `.hpp` компилировать не нужно, но можно указывать пути к ним).**
   Как мы знаем из Урока 1.4, `.hpp` файлы сами по себе не компилируются в `.o` — они вставляются препроцессором внутрь `.cpp`. При этом Ninja и CMake автоматически отслеживают все `#include "..."` внутри твоих `.cpp` файлов: если ты изменишь любой `.hpp` файл, Ninja сама поймет, какие именно `.cpp` файлы от него зависят, и пересоберет их!
3. **Генерация `compile_commands.json` для IDE и `clangd`:**
   Помнишь из Урока 1.3, что языковому серверу `clangd` нужен файл `compile_commands.json`?
   В CMake достаточно добавить одну строчку:
   ```cmake
   set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
   ```
   И в папке `build/` автоматически появится `compile_commands.json`, благодаря которому IDE будет идеально понимать весь твой проект!

---

## 4. Примеры использования

### 🟢 Легкий (Easy): Минимальный `CMakeLists.txt` для одной программы

```cmake
# 1. Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.20)

# 2. Название проекта и используемый язык (CXX = C++)
project(HelloCMake LANGUAGES CXX)

# 3. Задаем стандарт C++20 строго обязательно для всех таргетов
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Эквивалент строгого стандарта без нестандартных расширений

# 4. Создаем исполняемый таргет из двух .cpp файлов
add_executable(app
    main.cpp
    math_utils.cpp
)
```

Как собрать и запустить в терминале:
```bash
cmake -B build -G Ninja     # 1. Генерируем проект в папку build/ (используя быстрый Ninja)
cmake --build build         # 2. Компилируем и линкуем!
./build/app                 # 3. Запускаем готовый бинарник из папки build/
```

---

### 🟡 Средний (Medium): Сборка собственной библиотеки и её линковка к программе

Вот как в CMake решается твоя задача из прошлого урока (отделить библиотеку от `main.cpp`):

```cmake
cmake_minimum_required(VERSION 3.20)
project(AnalyticsApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Таргет №1: Создаем статическую библиотеку (libanalytics_core.a)
add_library(analytics_core STATIC
    sequence_math.cpp
    stats_printer.cpp
)

# Говорим, что заголовки (.hpp) этой библиотеки лежат в текущей папке проекта,
# и любой, кто подключит analytics_core (PUBLIC), тоже сможет делать #include этих заголовков!
target_include_directories(analytics_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Таргет №2: Создаем исполняемый файл только из main.cpp
add_executable(app main.cpp)

# Линкуем библиотеку analytics_core к нашему приложению app!
target_link_libraries(app PRIVATE analytics_core)
```

---

### 🔴 Тяжелый (Hard): Промышленный `CMakeLists.txt` со строгими флагами и санитайзерами

Чтобы не дублировать флаги `-Wall -Wextra -Wpedantic -Werror` и `-fsanitize=address,undefined` для каждого таргета, в современном C++ создают специальный интерфейсный таргет настроек (или функцию) или задают флаги таргетам:

```cmake
cmake_minimum_required(VERSION 3.20)
project(ProductionReadyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Для clangd в IDE!

# Создаем библиотеку
add_library(sequence_lib STATIC
    sequence_math.cpp
    stats_printer.cpp
)
target_include_directories(sequence_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Навешиваем строгие флаги компиляции и санитайзеры (PUBLIC — чтобы они передались и в app!)
target_compile_options(sequence_lib PUBLIC
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -g
    -O0
    -fsanitize=address,undefined
)

# Важно: санитайзеры требуют флага не только при компиляции (.cpp -> .o),
# но и при ЛИНКОВКЕ (.o -> app), чтобы линкер подключил рантайм ASan/UBSan!
target_link_options(sequence_lib PUBLIC
    -fsanitize=address,undefined
)

# Создаем приложение и подключаем библиотеку (все PUBLIC флаги подтянутся сами!)
add_executable(stats_app main.cpp)
target_link_libraries(stats_app PRIVATE sequence_lib)
```

> **Обрати внимание на `target_link_options(... -fsanitize=address,undefined)`!**
> Когда мы запускали одну длинную команду `clang++ -fsanitize=address ... main.cpp -o app`, драйвер `clang++` сам передавал флаг санитайзера и компилятору, и линкеру. В CMake компиляция и линковка разделены, поэтому `-fsanitize=address,undefined` обязательно указывается и в `target_compile_options`, и в `target_link_options`!

---

## 5. Где это применяется в реальном мире

1. **Единый стандарт в геймдеве, финансах и системном ПО**:
   На твоем Mac уже стоят **CMake (`4.4.5`)** и сверхбыстрый сборщик **Ninja** (проверено в`/opt/homebrew/bin/`). Связка `CMake + Ninja + Clang` — это буквально та самая связка, которой собирают браузер Chromium, движок V8, LLVM и серверные сервисы в BigTech.
2. **Подключение любых сторонних библиотек**:
   Когда в будущих модулях мы будем подключать `Raylib` (для 2D-игры), `Crow` (для веб-сервера), `GoogleTest` или `nlohmann/json`, мы будем делать это именно через `target_link_libraries` в `CMakeLists.txt`!

---

## 6. Практическое задание (`task-1-5`)

### Связь с предыдущими уроками
Мы берем наш многофайловый проект из `task-1-4`, прокачиваем его математику новой функцией из классических собеседований и переводим сборку с ручных команд `clang++` на профессиональный **CMake + Ninja** с выделением собственной статической библиотеки!

### Условие задачи
Создай директорию `tasks/task-1-5/`.

1. **Перенеси и расширь исходные файлы из `task-1-4`**:
   - В `stats_types.hpp` добавь в структуру `SequenceStats` четвертое поле:
     `long long fibonacci;` (n-е число Фибоначчи).
   - В `sequence_math.hpp` и `sequence_math.cpp` добавь новую функцию:
     `long long fibonacci(int n);`
     - Правила для `fibonacci(int n)`: при `n <= 0` возвращает `0`, при `n == 1` возвращает `1`, а для `n >= 2` вычисляет $F(n) = F(n-1) + F(n-2)$ **итеративно через цикл** (не рекурсией!). Например, для `n = 5`: ряд `0, 1, 1, 2, 3, 5` -> возвращает `5`; для `n = 10` -> возвращает `55`.
   - Обнови `build_sequence_stats(int n)` в `sequence_math.cpp`, чтобы она также заполняла поле `stats.fibonacci = fibonacci(n);`.
   - Обнови `stats_printer.cpp`, чтобы он выводил все 4 поля структуры (`Count`, `Sum of squares`, `Factorial`, `Fibonacci`). Не забудь поставить `#include "stats_printer.hpp"` на самую первую строчку `stats_printer.cpp`!
   - В `main.cpp` вызови `build_sequence_stats(10)` и `print_stats(stats)`.

2. **Создай файл `tasks/task-1-5/CMakeLists.txt`**:
   - Укажи `cmake_minimum_required(VERSION 3.20)` и `project(SequenceStatsCMake LANGUAGES CXX)`.
   - Включи стандарт C++20 без расширений (`CMAKE_CXX_STANDARD 20`, `CMAKE_CXX_STANDARD_REQUIRED ON`, `CMAKE_CXX_EXTENSIONS OFF`) и генерацию `CMAKE_EXPORT_COMPILE_COMMANDS ON`.
   - Создай статическую библиотеку **`sequence_stats_lib`** (`add_library(sequence_stats_lib STATIC sequence_math.cpp stats_printer.cpp)`).
   - Настрой для `sequence_stats_lib` (`PUBLIC`) директорию заголовков, флаги компиляции (`-Wall -Wextra -Wpedantic -Werror -g -O0 -fsanitize=address,undefined`) и флаги линковки (`-fsanitize=address,undefined`).
   - Создай исполняемый файл **`stats_app`** (`add_executable(stats_app main.cpp)`) и прилинкуй к нему библиотеку `sequence_stats_lib` через `target_link_libraries`.

3. **Проведи сборку и эксперимент для `tasks/task-1-5/REPORT.md`**:
   - Перейди в терминале в `tasks/task-1-5/` и выполни:
     ```bash
     cmake -B build -G Ninja
     cmake --build build
     ./build/stats_app
     ```
   - Загляни внутрь папки `build/` (`ls -la build`) и найди там сгенерированный файл статической библиотеки `libsequence_stats_lib.a` и файл `compile_commands.json`.
   - Проведи тест **инкрементальной сборки**: запусти `cmake --build build` второй раз подряд, ничего не меняя в коде. Что вывел Ninja? А теперь добавь пустую строчку или комментарий только в `main.cpp` и снова запусти `cmake --build build` — сколько файлов перекомпилировал Ninja (пересобралась ли библиотека `sequence_stats_lib`, или только `main.cpp`)?
   - Запиши ответы и вывод программы в `tasks/task-1-5/REPORT.md`. *(Папку `build/` можешь даже не удалять — наш `.gitignore` уже игнорирует `build/` в любом месте репозитория!)*

Жду твой первый CMake-проект!
