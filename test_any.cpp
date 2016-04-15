// Very simplist test, could be improved.

#include "any.hpp"
#include <memory>
#include <cstdio>

#define CHECK(x) ((x)? (void)(0) : (void(fprintf(stdout, "Failed at %d:%s: %s\n", __LINE__, __FILE__, #x)), std::exit(EXIT_FAILURE)))

int main()
{
    using linb::any;
    using linb::any_cast;
    using linb::bad_any_cast;

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
        CHECK(any().type() == typeid(void));
        CHECK(any(4).type() == typeid(int));
        CHECK(any(big_type()).type() == typeid(big_type));
        CHECK(any(1.5f).type() == typeid(float));
    }

    {
        bool except0 = false;
        bool except1 = false, except2 = false;
        bool except3 = false, except4 = false;

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

    //puts("...");
    //getchar();
}