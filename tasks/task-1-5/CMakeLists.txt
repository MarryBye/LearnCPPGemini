cmake_minimum_required(VERSION 3.20)
project(SequenceStatsCMake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(
    sequence_stats_lib 
    STATIC 
        sequence_math.cpp 
        stats_printer.cpp
)

target_include_directories(sequence_stats_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(sequence_stats_lib PUBLIC
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -g
    -O0
    -fsanitize=address,undefined
)

target_link_options(sequence_stats_lib PUBLIC
    -fsanitize=address,undefined
)

add_executable(
    stats_app 
        main.cpp
)

target_compile_options(stats_app PUBLIC
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -g
    -O0
    -fsanitize=address,undefined
)

target_link_options(stats_app PUBLIC
    -fsanitize=address,undefined
)

target_link_libraries(
    stats_app 
        sequence_stats_lib
)