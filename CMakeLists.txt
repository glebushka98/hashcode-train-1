cmake_minimum_required(VERSION 3.5)
project(algo)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp test.h )

add_executable(algo_test test.cpp)
add_executable(algo main.cpp)
target_link_libraries(algo_test pthread dl)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD} -Wall -Wextra -Wpedantic -g -O2")

SET_TARGET_PROPERTIES(algo_test PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DTEST")




