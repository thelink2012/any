#pragma once

#include "any.hpp"

#ifdef WIN32
#ifdef test_lib_EXPORTS
#define TESTLIB_API __declspec(dllexport)
#else
#define TESTLIB_API __declspec(dllimport)
#endif
#else
#define TESTLIB_API 
#endif

namespace shared_test_lib {
struct TESTLIB_API big_data final {
    explicit big_data();

    double a;
    float b;
    int data[1024];
};

struct TESTLIB_API small_data final {
    explicit small_data();
    int i;
};

TESTLIB_API linb::any createBigData();
TESTLIB_API linb::any createSmallData();

} // namespace shared_test_lib