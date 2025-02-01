cmake_minimum_required(VERSION 3.14)

project(emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/cpu.cpp
)
