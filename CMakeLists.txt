cmake_minimum_required(VERSION 3.21)
project(somnus_lib VERSION 3.21 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(${FILENAME} ${SOURCES})

if(CUSTOM_BUILD_TYPE STREQUAL "acrt")
    target_compile_options(${FILENAME} PRIVATE -std=c++23 -Wall -Wextra -pedantic -Werror -Og -ggdb -g)  
elseif(CUSTOM_BUILD_TYPE STREQUAL "dbg")
    target_compile_options(${FILENAME} PRIVATE -std=c++23 -Wall -Wextra -pedantic -Og -ggdb -g)  
elseif(CUSTOM_BUILD_TYPE STREQUAL "rls")
    target_compile_options(${FILENAME} PRIVATE -std=c++23 -O3 -flto -s -static)  
endif()