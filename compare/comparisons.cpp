#include <cassert>
#include <catch2/catch_test_macros.hpp>
#include <compare>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

using namespace std::literals;

TEST_CASE("safe comparing integral numbers")
{
    int x = -7;
    unsigned int y = 42;

    // CHECK(x < y);
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

    auto operator<=>(const Number& rhs) const = default;
};


struct FloatingNumber
{
    double value;
 
    bool operator==(const FloatingNumber&) const = default;

    std::strong_ordering operator<=>(const FloatingNumber& rhs) const
    {
        return std::strong_order(value, rhs.value);
    }
};


struct Value
{
    int value;

    constexpr Value(int v)
        : value{v}
    { }

    friend std::ostream& operator<<(std::ostream& out, const Value& v)
    {
        out << "Value{" << v.value << "}";
        return out;
    }

    bool operator==(const Value&) const = default;  // since C++20 - implicitly declared when <=> is defaulted

    auto operator<=>(const Value&) const = default; // since C++20

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
    CHECK(v1 != Value{15});  // !(v1 == v2)
    CHECK(5 == Value{5});
    CHECK(v1 == Number{10}); // v1.operator==(const Number& n);
    CHECK(Number{10} == v1); // rewriting: v1 == Number{10} -> v1.operator==(const Number& n);
    CHECK(v1 != Number(20));

    constexpr Value cv1{665};
    constexpr Value cv2{665};

    static_assert(cv1 == cv2);
}

TEST_CASE("X ==")
{
    X x1{42, "text"s, {1, 2, 3}, nullptr};
    X x2{42, "text"s, {1, 2, 3}};

    CHECK(x1 == x2);
    CHECK(x1 != X{665});
}

TEST_CASE("Value <=>")
{
    Value v1{10};
    Value v2{10};

    CHECK(Value{5} < Value{10});
    CHECK(v1 > Value{5});
    CHECK(v1 >= v2);

    std::vector<Value> vec = { Value{6}, Value{10}, Value{1}, Value{-7} };
    std::ranges::sort(vec);

    for(const auto& item : vec)
        std::cout << item << " ";
    std::cout << "\n";
}

TEST_CASE("operator <=>")
{
    int x = 10;
    assert(x <=> 10 == 0);
    assert(x <=> 20 < 0);
    assert(x <=> 5 > 0);

    SECTION("strong ordering")
    {
        std::strong_ordering result = 50 <=> 70;
        CHECK(result == std::strong_ordering::less);

        result = 50 <=> 50;
        CHECK(result == std::strong_ordering::equal);

        result = 50 <=> 30;
        CHECK(result == std::strong_ordering::greater);
    }

    SECTION("partial ordering")
    {
        auto result = 3.14 <=> 3.14;
        CHECK(result == std::partial_ordering::equivalent);

        result = 0.0 <=> -0.0;
        CHECK(result == std::partial_ordering::equivalent);

        result = std::numeric_limits<double>::quiet_NaN() <=> 5.77;
        CHECK(result == std::partial_ordering::unordered);
    }
}

//////////////////////////////////////////////////////////
// custom <=> operator

struct IntNan
{
    std::optional<int> value;

    bool operator==(const IntNan& rhs) const 
    {
        if (!value || !rhs.value)
        {
            return false;
        }
        return *value == *rhs.value;
    }

    std::partial_ordering operator<=>(const IntNan& rhs) const 
    {
        if (!value || !rhs.value)
        {
            return std::partial_ordering::unordered;
        }

        return *value <=> *rhs.value; // std::strong_ordering is implicitly converted to std::partial_ordering
    }   
};

TEST_CASE("IntNan")
{
    auto result = IntNan{2} <=> IntNan{4};
    CHECK(result == std::partial_ordering::less);

    result = IntNan{2} <=> IntNan{};
    CHECK(result == std::partial_ordering::unordered);

    CHECK(IntNan{4} == IntNan{4});
    CHECK(IntNan{4} != IntNan{5});

    result = IntNan{4} <=> IntNan{4};
    CHECK(result == std::partial_ordering::equivalent);
}

struct Human
{
    std::string name;
    int how_old;
    double height;

    bool operator==(const Human& rhs) const 
    {
        return std::tie(name, how_old) == std::tie(rhs.name, rhs.how_old);
    }

    std::strong_ordering operator<=>(const  Human& rhs) const
    {
        // custom impl
        // if (auto  cmp_result = name <=> rhs.name; cmp_result == 0)
        // {
        //     return how_old <=> rhs.how_old;            
        // }
        // else
        // {
        //     return cmp_result;
        // }

        // shortcut with std::tie
        return std::tie(name, how_old) <=> std::tie(rhs.name, rhs.how_old);
    }
};

TEST_CASE("Human - comparing")
{
    Human john1{"John", 33, 178.8};
    Human john2{"John", 33, 168.8};

    CHECK(john1 == john2);
    CHECK(john1 <= john2);
    CHECK(john1 > Human{"John", 22});
}
