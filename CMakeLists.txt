cmake_minimum_required(VERSION 3.14)

project(emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type." FORCE)
endif()

set(BUILD_SHARED_LIBS ON)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(wxBUILD_SHARED ON CACHE BOOL "" FORCE)

# these are sets to ensure that the build happens faster with threads and cacheing
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()



add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/wxWidgets EXCLUDE_FROM_ALL)


add_executable(${PROJECT_NAME}
    src/main.cpp
    src/console.cpp
    src/cpu.cpp
    src/memory.cpp
    src/os.cpp
    src/gpu.cpp
    src/controller.cpp
    src/disassembler.cpp
    src/gui.cpp
    src/vr.cpp
    src/filter.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-ggdb -O0>
    $<$<CONFIG:Release>:-O3 -Werror>
    $<$<CONFIG:Headless>: -O3 -Werror -DHEADLESS_BUILD>
)

# **Link wxWidgets and SDL2**
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 wx::core wx::base)

# Add a target to launch GDB
add_custom_target(debug
    COMMAND gdb -q ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "wxWidgets libraries: ${wxWidgets_LIBRARIES}")

