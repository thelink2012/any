project(any)
cmake_minimum_required(VERSION 2.8)
enable_testing()

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

add_executable(test_any test_any.cpp)
add_test(test_any test_any)
