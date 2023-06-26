#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <utility>

using namespace std::literals;

TEST_CASE("safe comparing integral numbers")
{
    int x = -7;
    unsigned int y = 42;

    //CHECK(x < y);
    CHECK(std::cmp_less(x, y));

    auto my_cmp_less = [](std::integral auto x, std::integral auto y) {
        return std::cmp_less(x, y);
    };

    CHECK(my_cmp_less(x, y));

    CHECK(std::in_range<size_t>(-1) == false);
    CHECK(std::in_range<size_t>(665) == true);
}

////////////////////////////////////////////////////////////////

struct Number
{
    int value;
};

struct Value
{
    int value;

    Value(int v) : value{v}
    {}

    friend std::ostream& operator<<(std::ostream& out, const Value& v)
    {
        out << "Value{" << v.value << "}";
        return out;
    }

    // bool operator==(const Value& rhs) const
    // {
    //     return value == rhs.value;
    // }

    bool operator==(const Value&) const = default; // since C++20

    bool operator==(const Number& n) const
    {
        return value == n.value;
    }
};

struct X
{   
    int id;
    std::string str;
    std::vector<int> vec;
    int* ptr;

    bool operator==(const X&) const = default; // since C++20
};

TEST_CASE("Value ==")
{
    Value v1{10};
    Value v2{10};

    CHECK(v1 == v2);
    CHECK(v1 != Value{15}); // !(v1 == v2)
    CHECK(5 == Value{5});
    CHECK(v1 == Number{10}); // v1.operator==(const Number& n);
    CHECK(Number{10} == v1); // rewriting: v1 == Number{10} -> v1.operator==(const Number& n);
    CHECK(v1 != Number(20));
}

TEST_CASE("X ==")
{
    X x1{42, "text"s, {1, 2, 3}, nullptr};
    X x2{42, "text"s, {1, 2, 3}};

    CHECK(x1 == x2);
    CHECK(x1 != X{665});
}