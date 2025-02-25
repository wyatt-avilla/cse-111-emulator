cmake_minimum_required(VERSION 3.14)

project(emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS ON)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/console.cpp
    src/cpu.cpp
    src/memory.cpp
    src/os.cpp
    src/gpu.cpp
    src/controller.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-ggdb -O0>
    $<$<CONFIG:Release>:-O3 -Werror>
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
