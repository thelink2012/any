# Any

This is a implementation of [N4562](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4562.html) std::experimental::any (merged into C++17) for C++11 compilers.

It contains a small object optimization for objects with a size of up to 2 words (such as  `int`, `float` and `std::shared_ptr`). Storing those objects in the container will not trigger a dynamic allocation.

For a easy to understand documentation, see [cppreference](http://en.cppreference.com/w/cpp/experimental/any), except our namespace is `linb`.

## Defines

You may additionally define the following preprocessor symbols (making the implementation non-standard):

  + `ANY_IMPL_NO_EXCEPTIONS`: This disables code paths that throw exceptions.
  + `ANY_IMPL_NO_RTTI`: This disables `any::type()`, thus removing all code paths using RTTI.
