cmake_minimum_required(VERSION 2.8)
project(any)
enable_testing()

set(CMAKE_CXX_STANDARD 11)

add_library(test_lib SHARED test_shared_lib.hpp test_shared_lib.cpp)

add_executable(test_any test_any.cpp)
target_link_libraries(test_any PRIVATE test_lib)
target_compile_options(test_any PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

add_test(test_any test_any)
