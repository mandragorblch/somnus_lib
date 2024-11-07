cmake_minimum_required(VERSION 3.10)
project(somnus_lib VERSION 0.0.1 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(main_test ${SOURCES})

target_compile_options(main_test PRIVATE -std=c++23 -Wall -Wextra -pedantic -Werror -O0 -ggdb -g)