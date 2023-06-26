#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std::literals;

int foo(int n)
{
    return 42 * n;
}

template <size_t N>
struct Foo
{
    constexpr static int value = 42 * N;
};

// template variable
template <size_t N>
constexpr static int Foo_v = Foo<N>::value;

//////////////////////////////////////////////

template <typename T>
struct IsVoid
{
    constexpr static bool value = false;
};

template <>
struct IsVoid<void>
{
    constexpr static bool value = true;
};

template <typename T>
constexpr static bool IsVoid_v = IsVoid<T>::value;

TEST_CASE("traits")
{
    CHECK(foo(2) == 84);
    static_assert(Foo<2>::value == 84);
    static_assert(Foo_v<2> == 84);

    using T1 = int;
    using T2 = void;

    static_assert(IsVoid_v<T1> == false);
    static_assert(IsVoid_v<T2> == true);
}

///////////////////////////////////////////
// IsPointer

template <typename T>
struct IsPointer
{
    static constexpr bool value = false;
};

template <typename T>
struct IsPointer<T*>
{
    static constexpr bool value = true;
};

template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

TEST_CASE("trait - IsPointer")
{
    using U = int;
    using V = int*;

    static_assert(IsPointer_v<U> == false);
    static_assert(IsPointer_v<V> == true);
}

TEST_CASE("constraints")
{
    //print(std::vector{1, 2, 3}, "vec");
    REQUIRE(true);
}

template <typename T>
concept Pointer = IsPointer_v<T>;

TEST_CASE("concepts")
{
    using U = int;
    using V = int*;

    static_assert(Pointer<U> == false);
    static_assert(Pointer<V> == true);

    REQUIRE(true);
}

/////////////////////////////////////////////////////////////////////
// templates & requirements

namespace BeforeCpp20
{
    template <typename T, typename = std::enable_if_t<!IsPointer_v<T>>>
    T max_value(T a, T b)
    {
        return (a < b) ? b : a;
    }

    template <typename T, typename = void, typename = std::enable_if_t<IsPointer_v<T>>>
    auto max_value(T a, T b)
    {
        return (*a < *b) ? *b : *a;
    }
} // namespace BeforeCpp20

namespace Cpp20
{
    template <typename T>
    T max_value(T a, T b)
    {
        return (a < b) ? b : a;
    }

    template <Pointer T>
    auto max_value(T a, T b)
    {
        return (*a < *b) ? *b : *a;
    }
} // namespace BeforeCpp20

TEST_CASE("max_value")
{
    using namespace Cpp20;

    CHECK(max_value(4, 6) == 6);
    CHECK(max_value(5.66, 8.88) == 8.88);

    int x1 = 10;
    int x2 = 42;

    CHECK(max_value(&x1, &x2) == 42);
}