// Very simplist test, could be better.

#include "any.hpp"
#include "test_shared_lib.hpp"
#include <memory>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#if defined(ANY_IMPL_NO_EXCEPTIONS) && defined(_MSC_VER)
# include <excpt.h>
#endif

#define CHECK(x) ((x)? (void)(0) : (void(fprintf(stdout, "Failed at %d:%s: %s\n", __LINE__, __FILE__, #x)), std::exit(EXIT_FAILURE)))

template<size_t N>
struct words
{
    void* w[N];
};

struct big_type
{
    char i_wanna_be_big[256];
    std::string value;

    big_type() :
        value(std::string(300, 'b'))
    {
        i_wanna_be_big[0] = i_wanna_be_big[50] = 'k';
    }

    bool check()
    {
        CHECK(value.size() == 300);
        CHECK(value.front() == 'b' && value.back() == 'b');
        CHECK(i_wanna_be_big[0] == 'k' && i_wanna_be_big[50] == 'k');
        return true;
    }
};

// small type which has nothrow move ctor but throw copy ctor
struct regression1_type
{
    const void* confuse_stack_storage = (void*)(0);
    regression1_type() {}
    regression1_type(const regression1_type&) {}
    regression1_type(regression1_type&&) noexcept {}
    regression1_type& operator=(const regression1_type&) { return *this; }
    regression1_type& operator=(regression1_type&&) { return *this; }
};

int main()
{
    using linb::any;
    using linb::any_cast;
    using linb::bad_any_cast;
    using linb::make_any;

    {
        any x = 4;
        any y = big_type();
        any z = 6;

        CHECK(any().empty());
        CHECK(!any(1).empty());
        CHECK(!any(big_type()).empty());

        CHECK(!x.empty() && !y.empty() && !z.empty());
        y.clear();
        CHECK(!x.empty() && y.empty() && !z.empty());
        x = y;
        CHECK(x.empty() && y.empty() && !z.empty());
        z = any();
        CHECK(x.empty() && y.empty() && z.empty());
    }

    {
        any o1 = make_any<std::vector<int>>({2, 2});
        any o2 = make_any<std::vector<int>>(2, 2);
        CHECK(any_cast<std::vector<int>>(o1) == any_cast<std::vector<int>>(o2));
    }
#ifndef ANY_IMPL_NO_RTTI
    {
        CHECK(any().type() == typeid(void));
        CHECK(any(4).type() == typeid(int));
        CHECK(any(big_type()).type() == typeid(big_type));
        CHECK(any(1.5f).type() == typeid(float));
    }
#endif

    {
        bool except0 = false;
        bool except1 = false, except2 = false;
        bool except3 = false, except4 = false;

#ifndef ANY_IMPL_NO_EXCEPTIONS
        try {
            any_cast<int>(any());
        }
        catch(const bad_any_cast&) {
            except0 = true;
        }

        try {
            any_cast<int>(any(4.0f));
        }
        catch(const bad_any_cast&) {
            except1 = true;
        }

        try {
            any_cast<float>(any(4.0f));
        }
        catch(const bad_any_cast&) {
            except2 = true;
        }

        try {
            any_cast<float>(any(big_type()));
        }
        catch(const bad_any_cast&) {
            except3 = true;
        }

        try {
            any_cast<big_type>(any(big_type()));
        }
        catch(const bad_any_cast&) {
            except4 = true;
        }
#elif _MSC_VER
        // we can test segmentation faults with msvc

# ifdef _CPPUNWIND
#   error Must use msvc compiler with exceptions disabled (no /EHa, /EHsc, /EHs)
# endif

        __try {
            any_cast<int>(any());
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            except0 = true;
        }
        __try {
            any_cast<int>(any(4.0f));
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            except1 = true;
        }
        __try {
            any_cast<float>(any(4.0f));
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            except2 = true;
        }
        __try {
            any_cast<float>(any(big_type()));
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            except3 = true;
        }
        __try {
            any_cast<big_type>(any(big_type()));
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            except4 = true;
        }
#endif

        CHECK(except0 == true);
        CHECK(except1 == true && except2 == false);
        CHECK(except3 == true && except4 == false);
    }

    {
        any i4 = 4;
        any i5 = 5;
        any f6 = 6.0f;
        any big1 = big_type();
        any big2 = big_type();
        any big3 = big_type();

        CHECK(any_cast<int>(&i4) != nullptr);
        CHECK(any_cast<float>(&i4) == nullptr);
        CHECK(any_cast<int>(i5) == 5);
        CHECK(any_cast<float>(f6) == 6.0f);
        CHECK(any_cast<big_type>(big1).check()
            && any_cast<big_type>(big2).check()
            && any_cast<big_type>(big3).check());
    }

    {
        std::shared_ptr<int> ptr_count(new int);
        std::weak_ptr<int> weak = ptr_count;
        any p0 = 0;

        CHECK(weak.use_count() == 1);
        any p1 = ptr_count;
        CHECK(weak.use_count() == 2);
        any p2 = p1;
        CHECK(weak.use_count() == 3);
        p0 = p1;
        CHECK(weak.use_count() == 4);
        p0 = 0;
        CHECK(weak.use_count() == 3);
        p0 = std::move(p1);
        CHECK(weak.use_count() == 3);
        p0.swap(p1);
        CHECK(weak.use_count() == 3);
        p0 = 0;
        CHECK(weak.use_count() == 3);
        p1.clear();
        CHECK(weak.use_count() == 2);
        p2 = any(big_type());
        CHECK(weak.use_count() == 1);
        p1 = ptr_count;
        CHECK(weak.use_count() == 2);
        ptr_count = nullptr;
        CHECK(weak.use_count() == 1);
        p1 = any();
        CHECK(weak.use_count() == 0);
    }

    {
        auto is_stack_allocated = [](const any& a, const void* obj1) {
            uintptr_t a_ptr = (uintptr_t)(&a);
            uintptr_t obj   = (uintptr_t)(obj1);
            return (obj >= a_ptr && obj < a_ptr + sizeof(any));
        };

        //static_assert(sizeof(std::unique_ptr<big_type>) <= sizeof(void*) * 1, "unique_ptr too big");
        static_assert(sizeof(std::shared_ptr<big_type>) <= sizeof(void*) * 2, "shared_ptr too big");

        any i = 400;
        any f = 400.0f;
        //any unique = std::unique_ptr<big_type>(); -- must be copy constructible
        any shared = std::shared_ptr<big_type>();
        any rawptr = (void*)(nullptr);
        any big = big_type();
        any w2 = words<2>();
        any w3 = words<3>();

        CHECK(is_stack_allocated(i, any_cast<int>(&i)));
        CHECK(is_stack_allocated(f, any_cast<float>(&f)));
        CHECK(is_stack_allocated(rawptr, any_cast<void*>(&rawptr)));
        //CHECK(is_stack_allocated(unique, any_cast<std::unique_ptr<big_type>>(&unique)));
        CHECK(is_stack_allocated(shared, any_cast<std::shared_ptr<big_type>>(&shared)));
        CHECK(!is_stack_allocated(big, any_cast<big_type>(&big)));
        CHECK(is_stack_allocated(w2, any_cast<words<2>>(&w2)));
        CHECK(!is_stack_allocated(w3, any_cast<words<3>>(&w3)));

        // Regression test for GitHub Issue #1
        any r1 = regression1_type();
        CHECK(is_stack_allocated(r1, any_cast<const regression1_type>(&r1)));
    }

    // correctly stored decayed and retrieved in decayed form
    {
        const int i = 42;
        any a = i;

        // retrieve 
        CHECK(any_cast<int>(&a) != nullptr);
        CHECK(any_cast<const int>(&a) != nullptr);


#ifndef ANY_IMPL_NO_EXCEPTIONS
        // must not throw
        bool except1 = false, except2 = false, except3 = false;

        // same with reference to any
        try {
            any_cast<int>(a);
        }
        catch(const bad_any_cast&) {
            except1 = true;
        }
        try {
            any_cast<const int>(a);
        }
        catch(const bad_any_cast&) {
            except2 = true;
        }
        try {
            any_cast<const int>(std::move(a));
        }
        catch(const bad_any_cast&) {
            except3 = true;
        }

        CHECK(except1 == false);
        CHECK(except2 == false);
        CHECK(except3 == false);
#endif
    }

    {
      bool except1 = false, except2 = false;
      auto big_any = shared_test_lib::createBigData();
      auto small_any = shared_test_lib::createSmallData();
      try {
          any_cast<shared_test_lib::big_data>(big_any);
      } catch (const bad_any_cast &) {
          except1 = true;
      }
      try {
          any_cast<shared_test_lib::small_data>(small_any);
      } catch (const bad_any_cast &) {
          except2 = true;
      }

#ifndef ANY_IMPL_NO_RTTI
      CHECK(except1 == false);
      CHECK(except2 == false);
#else
      CHECK(except1 == true);
      CHECK(except2 == true);
#endif

    }
}