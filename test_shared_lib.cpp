#include "test_shared_lib.hpp"

namespace shared_test_lib {

    big_data::big_data() : a(10), b(11), data() {}

    small_data::small_data() : i(1) {}

    linb::any createBigData() { return big_data{}; }

    linb::any createSmallData() { return small_data{}; }

}

