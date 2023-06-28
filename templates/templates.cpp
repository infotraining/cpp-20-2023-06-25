#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <source_location>

using namespace std::literals;

auto create_caller(auto f, auto... args)
{
    return [f, ... args = std::move(args)]() -> decltype(auto) {
        return f(args...);
    };
}

TEST_CASE("templates & lambda expressions")
{
    auto printer1 = [](const auto& coll) {
        std::cout << coll.size() << "\n";
    };

    auto printer2 = []<typename T>(const std::vector<T>& coll) {
        std::cout << coll.size() << "\n";
    };

    auto emplace_to_vec = []<typename T, typename... TArgs>(std::vector<T>& coll, TArgs&&... args) {
        coll.emplace_back(std::forward<TArgs>(args)...);
    };

    decltype(emplace_to_vec) emplacer; // OK - since C++20

    auto cmp_by_value = [](auto a, auto b) { return *a < *b; };

    std::set<int*, decltype(cmp_by_value)> my_set;

    auto calculate = create_caller(std::plus{}, 3, 5);
 
    CHECK(calculate() == 8);
}

struct Data
{
    int a;
    std::vector<int> vec;
    double d;
};

TEST_CASE("aggregates")
{
    Data d1{.a = 1, .d = 3.14}; // designated initializers
    //Data d1{.d = 3.14, .a = 1, }; // ERROR

    Data d2(1, {1, 2, 3});

    auto ptr = std::make_unique<Data>(1, std::vector{1, 2, 3}, 3.14);

    auto sl = std::source_location::current();
 
    std::cout << "file: " << sl.file_name() << "\n";
    std::cout << "function: " << sl.function_name() << "\n";
    std::cout << "line/col: " << sl.line() << "\n";
}

//////////////////////////////////////////////////////////
// NTTP


template <auto Factor>
auto scale(auto x)
{
    return Factor * x;
}

TEST_CASE("NTTP - floating points")
{
    CHECK(scale<2>(3) == 6);

    CHECK(scale<2.0>(3.14) == 6.28);
}

///////////////////////////////////////
// structs as NTTP

struct Tax
{
    double value;

    constexpr Tax(double value) : value{value}
    {
        assert(value > 0 && value < 1.0);
    }
};

template <Tax Vat>
double calc_gross_price(double net_price)
{
    return net_price + net_price * Vat.value;
}

TEST_CASE("struct as NTTP")
{
    constexpr Tax vat_pl{0.23};
    constexpr Tax vat_ger{0.19};

    CHECK(calc_gross_price<vat_pl>(100.0) == 123.0);
    CHECK(calc_gross_price<vat_ger>(100.0) == 119.0);
}

template <size_t N>
struct Str
{
    char value[N];

    constexpr Str(const char (&str)[N])
    {
        std::copy(str, str + N, value);
    }

    friend std::ostream& operator<<(std::ostream& out, const Str& str)
    {
        out << str.value;

        return out;
    }
};

template <Str Prefix>
class Logger
{
public:
    void log(const std::string& msg)
    {
        std::cout << Prefix << msg << "\n";
    }
};

TEST_CASE("strings as NTTP")
{
    Logger<">: "> logger1;
    logger1.log("Start");
    logger1.log("Stop");

    Logger<">>: "> logger2;
    logger2.log("Start");
    logger2.log("Stop");
}

template <std::invocable auto GetVat>
double calc_gross_price(double net_price)
{
    return net_price + net_price * GetVat();
}

TEST_CASE("lambda as NTTP")
{
    CHECK(calc_gross_price<[]{ return 0.23; }>(100.0) == 123.0);

    constexpr static auto vat_ger = [] { return 0.19; };
    CHECK(calc_gross_price<vat_ger>(100.0) == 119.0);
}