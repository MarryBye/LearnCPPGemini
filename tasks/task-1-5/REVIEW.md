# Code Review: `task-1-5`

**Статус**: ✅ Принято

---

## 1. Что сделано хорошо
- **Чистая итеративная реализация `fibonacci(int n)`**: работает за $O(n)$ по времени и $O(1)$ по памяти, корректно обработаны граничные случаи `n <= 0` и `n == 1`.
- **Рабочий `CMakeLists.txt` с генератором Ninja**: настроены стандарт C++20 без расширений, экспорт `compile_commands.json`, статическая библиотека `sequence_stats_lib` и исполняемый файл `stats_app`.
- **Эксперимент с инкрементальной сборкой**: проверено поведение `ninja: no work to do.` и частичная пересборка при изменении одного файла.

---

## 2. Ответ на твой вопрос из `REPORT.md`: «Зачем выделять `.cpp` файлы в библиотеку `add_library`, если можно просто закинуть всё в `add_executable`?»

Ты спросил:
> *«А если бы я не выделял как библиотеку вспомогательные .cpp файлы, то программа работала бы так же? А в чем смысл ее выделения в целом? Для передачи?»*

Да, если написать просто `add_executable(stats_app main.cpp sequence_math.cpp stats_printer.cpp)`, итоговый бинарник `stats_app` получится точно таким же!
Но в реальных индустриальных проектах в `add_executable` почти всегда оставляют **только один файл `main.cpp`**, а весь остальной код выносят в `add_library`. На это есть **3 фундаментальные причины**:

### Причина 1: Юнит-тестирование (Самая главная причина!)
Представь, что завтра ты захочешь написать автотесты (`tests.cpp`), которые проверяют твои функции `fibonacci` и `factorial`.
Чтобы запустить тесты, нужно собрать второй исполняемый файл `unit_tests`, у которого **своя собственная функция `main()`**!
- Если весь твой код свален в `add_executable(stats_app ...)`, ты **не сможешь** подключить `stats_app` к тестам — в C++ нельзя линковать два файла с функцией `main()` (линкер упадет с `duplicate symbol _main`). Тебе пришлось бы компилировать одни и те же `.cpp` файлы дважды!
- А когда вся логика вынесена в библиотеку `sequence_stats_lib` (внутри которой нет `main()`), ты компилируешь её **один раз** и подключаешь одной строчкой сразу к двум программам:
  ```cmake
  add_executable(stats_app main.cpp)
  target_link_libraries(stats_app PRIVATE sequence_stats_lib)

  add_executable(unit_tests tests.cpp)
  target_link_libraries(unit_tests PRIVATE sequence_stats_lib)
  ```
  (Именно так мы будем делать уже в Уроке 1.6!).

### Причина 2: Несколько приложений поверх одного ядра
В реальном продукте часто бывает одно ядро (`engine_lib`), и поверх него в одном репозитории собираются:
- консольная утилита (`cli_app`),
- сервер (`server_app`),
- бенчмарк для замера скорости (`benchmark_app`),
- обертка для Python (`pybind_module`).
Все они просто линкуют готовую `engine_lib`!

### Причина 3: Почему при изменении `stats_printer.cpp` Ninja написал `[3/3]`?
Ты заметил в отчете: *«поменяв `stats_printer.cpp`, пересобралось 3 файла»*.
Посмотри, какие именно 3 шага сделал Ninja:
1. `[1/3] Building CXX object ... stats_printer.cpp.o` — скомпилировал **только один** `.cpp` файл (`sequence_math.cpp` и `main.cpp` даже не компилировались!).
2. `[2/3] Linking CXX static library libsequence_stats_lib.a` — обновил архив библиотеки.
3. `[3/3] Linking CXX executable stats_app` — перелинковал финальный бинарник.

---

## 3. Архитектурные и стилистические замечания (на будущее)

1. **Избыточное дублирование флагов в `CMakeLists.txt` (строки 37–49)**:
   Так как ты указал `target_compile_options(sequence_stats_lib PUBLIC ...)` и `target_link_options(sequence_stats_lib PUBLIC ...)` с ключевым словом **`PUBLIC`**, то при вызове `target_link_libraries(stats_app PRIVATE sequence_stats_lib)` все эти флаги **автоматически передаются в `stats_app`**! Поэтому дублировать `target_compile_options` и `target_link_options` второй раз для `stats_app` не нужно.
2. **Явный модификатор `PRIVATE` в `target_link_libraries` (строка 51)**:
   Вместо `target_link_libraries(stats_app sequence_stats_lib)` всегда пиши явно:
   `target_link_libraries(stats_app PRIVATE sequence_stats_lib)`.
3. **Объявление переменной `c` в `fibonacci` (`sequence_math.cpp`, строка 31)**:
   Вместо объявления неинициализированной переменной `long long a = 0, b = 1, c;` до цикла лучше объявлять `c` прямо внутри цикла в момент инициализации:
   ```cpp
   long long a = 0;
   long long b = 1;
   for (int i = 2; i <= n; ++i) {
       long long c = a + b;
       a = b;
       b = c;
   }
   ```
   В C++ действует правило: **минимизируй область видимости переменных и никогда не создавай неинициализированных переменных**.
