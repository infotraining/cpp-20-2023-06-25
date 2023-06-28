#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std::literals;

namespace Traits
{
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
} // namespace Traits

TEST_CASE("traits")
{
    using namespace Traits;
    
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
    // print(std::vector{1, 2, 3}, "vec");
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
    namespace ConceptWithTrait
    {
        template <typename T>
        concept Pointer = IsPointer_v<T>;
    }

    template <typename T>
    concept Pointer = requires(T ptr) {
        *ptr;
        ptr == nullptr;
    };

    namespace ver_1
    {
        template <typename T>
        T max_value(T a, T b)
        {
            return (a < b) ? b : a;
        }

        template <typename T>
            requires IsPointer_v<T>
        auto max_value(T a, T b)
        {
            return (*a < *b) ? *b : *a;
        }

        void foo(auto n)
            requires(sizeof(n) <= 16)
        {
            std::cout << "foo for small objects\n";
        }
    } // namespace ver_1

    inline namespace ver_2
    {
        template <typename T>
        T max_value(T a, T b)
        {
            return (a < b) ? b : a;
        }

        template <typename T>
            requires Pointer<T>
        auto max_value(T a, T b)
        {
            return (*a < *b) ? *b : *a;
        }

    } // namespace ver_2

    namespace ver_3
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

        template <Pointer T>
        auto max_value(T a, T b)
            requires(sizeof(std::remove_reference_t<decltype(*a)>) <= 8)
        {
            return (*a < *b) ? *b : *a;
        }
    } // namespace ver_3

    namespace ver_4
    {
        auto max_value(auto a, auto b)
        {
            return (a < b) ? b : a;
        }

        auto max_value(Pointer auto a, Pointer auto b)
            requires(std::is_same_v<decltype(a), decltype(b)>)
        {
            return (*a < *b) ? *b : *a;
        }
    } // namespace ver_4
} // namespace Cpp20

TEST_CASE("concepts")
{
    using namespace Cpp20;

    using U = int;
    using V = int*;

    static_assert(Pointer<U> == false);
    static_assert(Pointer<V> == true);

    REQUIRE(true);
}

TEST_CASE("max_value")
{
    using namespace Cpp20;

    CHECK(max_value(4, 6) == 6);
    CHECK(max_value(5.66, 8.88) == 8.88);

    int x1 = 10;
    int x2 = 42;

    CHECK(max_value(&x1, &x2) == 42);

    auto ptr_a = std::make_shared<int>(10);
    auto ptr_b = std::make_shared<int>(20);

    CHECK(max_value(ptr_a, ptr_b) == 20);
}

template <typename T>
struct Wrapper
{
    T value;

    void print() const
    {
        std::cout << "value: " << value << "\n";
    }

    void print() const
        requires std::ranges::range<T>
    {
        std::cout << "values: [ ";
        for (const auto& item : value)
            std::cout << item << " ";
        std::cout << "]\n";
    }
};

// deduction (obsolete since C++20)
// template <typename T>
// Wrapper(T) -> Wrapper<T>;

TEST_CASE("Wrapper - class template with requires for method")
{
    Wrapper w1{42}; // CTAD
    w1.print();

    Wrapper w2{std::vector{1, 2, 3, 4}};
    w2.print();
}

std::unsigned_integral auto get_number()
{
    return 42u;
}

TEST_CASE("auto placeholder + concept")
{
    std::convertible_to<uint32_t> auto n = get_number();
}

//////////////////////////////////////////
// requires expression

template <typename T>
concept BigInt = std::integral<T> && requires(T obj) {
    requires sizeof(obj) > 4;
};

// static_assert(BigInt<char>);
// static_assert(BigInt<std::string>);
static_assert(BigInt<int64_t>);

void add_to_container(auto& container, auto&& item)
{
    if constexpr (requires { container.push_back(std::forward<decltype(item)>(item)); })
    {
        container.push_back(std::forward<decltype(item)>(item));
    }
    else
    {
        container.insert(std::forward<decltype(item)>(item));
    }
}

TEST_CASE("requires in constexpr if")
{
    std::list<int> lst;
    add_to_container(lst, 5);

    std::set<int> my_set;
    add_to_container(my_set, 5);
}

/////////////////////////////////////////////////////////////////
// concept subsumation

struct BoundingBox
{
    int w, h;
};

struct Color
{
    uint8_t r, g, b;
};

template <typename T>
concept Shape = requires(T obj) {
    {
        obj.box()
    } -> std::same_as<BoundingBox>;
    obj.draw();
};

template <typename T>
concept ShapeWithColor = Shape<T> && requires(T obj, Color c) {
    obj.set_color(c);
    {
        obj.get_color()
    } -> std::same_as<Color>;
};

void render(const Shape auto& shp)
{
    shp.draw();
}

void render(ShapeWithColor auto&& shp)
{
    shp.set_color({0, 0, 0});
    shp.draw();
}

struct IShape
{
    virtual void draw() const = 0;
    virtual ~IShape() = default;
};

struct Rect
{
    int w, h;

    void draw() const
    {
        std::cout << "Rect::draw()\n";
    }

    BoundingBox box() const
    {
        return BoundingBox{w, h};
    }

    // void set_color(Color c)
    // {
    //     std::cout << "Setting color\n";
    // }

    // Color get_color() const
    // {
    //     return Color{};
    // }
};

// static_assert(ShapeWithColor<Rect>);

TEST_CASE("subsuming concepts")
{
    render(Rect{10, 20});
}

template <typename T>
concept SignedIntegral = std::integral<T> && std::is_signed_v<T>;

template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

void bar(SignedIntegral auto n)
{
    std::cout << "bar(SignedIntegral)\n";
}

void bar(std::integral auto n)
{
    std::cout << "bar(integral)\n";
}

void bar(Arithmetic auto n)
{
    std::cout << "bar(Arithmetic)\n";
}

TEST_CASE("bar & subsumation")
{
    bar(42);
}
