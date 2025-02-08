cmake_minimum_required(VERSION 3.14)

project(emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/console.cpp
    src/cpu.cpp
    src/memory.cpp
    src/os.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-ggdb -O0>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:CI>:-Werror>
)
