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

////////////////////////////////////////////////////////

namespace MyNamespace
{
    struct Vector
    {
        int items[10];
    };

    int* begin(Vector&)
    {
        return nullptr;
    }

    int* end(Vector&)
    {
        return nullptr;
    }
}

template <typename TContainer>
void some_algorithm(TContainer& container)
{
    // using std::begin;
    // using std::end;

    // for(auto it = begin(container); it != end(container); ++it)
    for(auto it = std::ranges::begin(container); it != std::ranges::end(container); ++it)
        std::cout << *it << " ";
    std::cout << "\n";    
}

TEST_CASE("std::begin vs ranges::begin")
{
    std::vector<int> vec = {1, 2, 3, 4};
    int tab[4] = {1, 2, 3, 4};
    MyNamespace::Vector my_vec{1, 2, 3, 4};

    some_algorithm(vec);
}

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

// template <std::ranges::range TContainer>
// void print_all_items(TContainer&& coll)
// {
//     std::cout << "void print_all_items(TContainer&& coll)\n";
//     for(const auto& item : coll)
//         std::cout << item << " ";
//     std::cout << "\n";
// }

void print_all_items(std::ranges::view auto coll)
{
    std::cout << "void print_all_items(std::ranges::view auto coll)\n";
    for(const auto& item : coll)
        std::cout << item << " ";
    std::cout << "\n";
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

        auto my_range = std::views::take(std::views::iota(1), 20);

        helpers::print(my_range, "my_range");

        auto data = std::views::iota(1)
                        | std::views::take(20)
                        | std::views::filter([](int x) { return x % 2 == 0; })
                        | std::views::transform([](int x) { return x * x; });

        helpers::print(data, "data");

        print_all_items(data);
    }

    SECTION("views - reference semantics")
    {
        std::vector vec = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9, 10};

        print_all_items(std::views::all(vec));


        for(auto& item : vec | std::views::drop(5))
            item = 0;

        helpers::print(vec, "vec");
    }

    SECTION("spliting string")
    {
        const char* text = "abc def ghi";

        std::vector<std::string_view> tokens;

        auto splitted_ranges = std::string_view(text) | std::views::split(" "sv);

        for(auto&& rng : splitted_ranges)
        {
            tokens.emplace_back(&*rng.begin(), std::ranges::distance(rng));
            // tokens.emplace_back(rng); // since C++23
        }

        CHECK(tokens == std::vector{"abc"sv, "def"sv, "ghi"sv});
    }
}

std::vector<int> get_data()
{
    // std::vector vec{1, 2, 3, 42, 22};
    // return vec; // lvale - optional NRVO

    return {1, 2, 3, 42, 22}; // prvalue - must be RVO
}

TEST_CASE("borrowed iterators")
{
    SECTION("dangling iterator")
    {
        auto pos = std::ranges::find(get_data(), 42);

        //CHECK(*pos == 42); // Compilation ERROR - dangling iterator
    }

    SECTION("valid iterator")
    {
        auto&& data = get_data();
        auto pos = std::ranges::find(data, 42);

        REQUIRE(*pos == 42);
    }
}