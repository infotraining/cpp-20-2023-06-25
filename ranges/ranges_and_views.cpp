#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <helpers.hpp>
#include <iostream>
#include <iterator>
#include <list>
#include <ranges>
#include <string>
#include <vector>

using namespace std::literals;

/////////////////////
// Sentinel

template <auto Value>
struct EndValue
{
    bool operator==(std::input_or_output_iterator auto it) const
    {
        return *it == Value;
    }
};

TEST_CASE("algorithms")
{
    SECTION("passing range")
    {
        auto data = helpers::create_numeric_dataset<100>(42);
        helpers::print(data, "data");

        std::ranges::sort(data);

        helpers::print(data, "data asc");

        std::ranges::sort(data, std::greater{});

        helpers::print(data, "data desc");
    }

    SECTION("projections")
    {
        std::vector words = {"twenty-two"s, "a"s, "abc"s, "b"s, "one"s, "aa"s};

        std::ranges::sort(words, std::less{}, /*projection*/ [](const auto& w) { return w.size(); });

        helpers::print(words, "words");
    }

    SECTION("iterator & sentinel")
    {
        std::vector data = {5, 423, 665, 1, 235, 42, 6, 345, 33};

        helpers::print(data, "data");

        std::ranges::sort(/*iterator*/ data.begin(), /*sentinel*/ EndValue<42>{});

        helpers::print(data, "data");

        EndValue<'\0'> null_term; // EndValue<char = '\0'>

        auto& txt = "acbgdef\0ajdhfgajsdhfgkasdjhfg";
        auto txt_array = std::to_array("abcgdef\0ajdhfgajsdhfgkasdjhfg");

        std::ranges::sort(std::ranges::begin(txt_array), null_term, std::greater{});

        helpers::print(std::ranges::subrange(txt_array.begin(), null_term), "txt_array after sort");

        auto pos = std::ranges::find(data.begin(), std::unreachable_sentinel, 665);
        CHECK(*pos == 665);
    }
}

TEST_CASE("views")
{
    std::list vec = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9, 10};

    SECTION("all")
    {
        auto all_items = std::views::all(vec);

        helpers::print(all_items, "all items");
    }

    SECTION("counted")
    {
        auto first_half = std::views::counted(vec.begin(), vec.size() / 2);

        for (auto& item : first_half)
            item *= 2;

        helpers::print(vec, "vec");
    }

    SECTION("iota")
    {
        helpers::print(std::views::iota(1, 20), "iota");
    }
}

