#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <numeric>
#include <ranges>
#include <algorithm>

using namespace std::literals;

int runtime_func(int x)
{
    return x * x;
}

constexpr int constexpr_func(int x)
{
    return x * x;
}

consteval int consteval_func(int x)
{
    return x * x;
}

TEST_CASE("consteval")
{
    int x = 2;
    CHECK(runtime_func(x) == 4);   // runtime call

    CHECK(constexpr_func(x) == 4); // runtime call

    constexpr int cx = 652;
    constexpr int sq665 = constexpr_func(cx); // compile-time call

    constexpr int sq777 = consteval_func(777);
    int sq888 = consteval_func(888);
}

void compile_time_error() // runtime function
{ }

consteval int next_two_digit_value(int value)
{
    if (value < 9 || value >= 99)
    {
        compile_time_error();
    }

    return ++value;
}

static_assert(next_two_digit_value(50) == 51);
//static_assert(next_two_digit_value(8) == 9);

consteval int pow_2(int x)
{
    return x * x;
}

TEST_CASE("using consteval")
{
    std::array squares = { pow_2(1), pow_2(2), pow_2(3) };
}

constexpr int len(const char* s)
{
    if (std::is_constant_evaluated())
    {
        // compile-time friendly code
        int idx = 0;
        while (s[idx] != '\0')
            ++idx;
        return idx;
    }
    else
    {
       return std::strlen(s); // function called at runtime
    }
}

TEST_CASE("is_constant_evaluated")
{
    constexpr auto size = len("abc");
}

constinit int i = 42;

consteval int get_id()
{
    return 665;
}

struct MyType
{
    static inline constinit long id = get_id();
};

TEST_CASE("constinit")
{
    ++i;

    CHECK(MyType::id == 665);
}

template <size_t N>
constexpr auto create_powers()
{
    std::array<uint32_t, N> powers{};

    std::iota(powers.begin(), powers.end(), 1); // std algorithms are constexpr

    std::ranges::transform(powers, powers.begin(), [](int x) { return x * x; }); // ranges algorithms are constexpr

    return powers;
}

TEST_CASE("constexpr functions in C++20")
{
    constexpr auto powers_lookup_table = create_powers<100>();
}

template <std::ranges::input_range... TRng_>
constexpr auto avg_for_unique(const TRng_&... rng)
{
    using TElement = std::common_type_t<std::ranges::range_value_t<TRng_>...>;

    std::vector<TElement> vec;                            // empty vector
    vec.reserve((rng.size() + ...));                      // reserve a buffer - fold expression C++17
    (vec.insert(vec.end(), rng.begin(), rng.end()), ...); // fold expression C++17

    // sort items
    std::ranges::sort(vec); // std::sort(vec.begin(), vec.end());

    // create span of unique_items
    auto new_end = std::unique(vec.begin(), vec.end());
    std::span unique_items{vec.begin(), new_end};

    // calculate sum of unique items
    auto sum = std::accumulate(unique_items.begin(), unique_items.end(), TElement{});

    return sum / static_cast<double>(unique_items.size());
}

TEST_CASE("avg for unique")
{
    constexpr std::array lst1 = {1, 2, 3, 4, 5};
    constexpr std::array lst2 = {5, 6, 7, 8, 9};

    constexpr auto avg = avg_for_unique(lst1, lst2);

    std::cout << "AVG: " << avg << "\n";
}