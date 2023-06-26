#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <format>

using namespace std::literals;

struct Rating
{
    int value;

    bool operator==(const Rating& other) const
    {
        return value == other.value;
    }

    bool operator<(const Rating& other) const
    {
        return value < other.value;
    }
};

struct Gadget
{
    std::string name;
    double price;

    bool operator==(const Gadget& other) const = default;
    
    std::strong_ordering operator<=>(const Gadget& other) const
    {
        if (std::strong_ordering order = name <=> other.name; order == 0)
        {
            return std::strong_order(price, other.price);
        }
        else
            return order;
    }
};

struct SuperGadget : Gadget
{
    Rating rating;

    //std::strong_ordering operator<=>(const SuperGadget& other) const = default;

    bool operator==(const SuperGadget&) const = default;

    std::strong_ordering operator<=>(const SuperGadget& other) const
    {
        if (auto cmp_result = (*this).Gadget::operator<=>(other); cmp_result != 0)
        {
            return cmp_result;
        }

        return std::compare_strong_order_fallback(rating, other.rating); // it will use == & < tp establish strong_ordering
    }
};

TEST_CASE("Gadget - write custom operator <=> - stronger category than auto detected")
{
    SECTION("==")
    {
        CHECK(Gadget{"ipad", 1.0} == Gadget{"ipad", 1.0});
    }
    
    SECTION("<=>")
    {
        std::strong_ordering result = Gadget{"ipad", 1.0} <=> Gadget{"ipad", 1.0};
        CHECK(result == std::strong_ordering::equal);
    }
}

TEST_CASE("SuperGadget - write custom operator <=> - member without compare-three-way operator")
{
    CHECK(SuperGadget{{"ipad", 1.0}, Rating{1}} != SuperGadget{{"ipad", 1.0}, Rating{2}});

    auto result = SuperGadget{{"ipad", 1.0}, Rating{1}} <=> SuperGadget{{"ipad", 1.0}, Rating{2}};
    CHECK(result == std::strong_ordering::less);    
}


//////////////////////////////////////////////////////////////////////////////////////////////////////


enum class RatingValue : uint8_t { very_poor = 1, poor, satisfactory, good, very_good, excellent};

struct RatingStar
{
public:
    RatingValue value;

    explicit RatingStar(RatingValue rating_value) : value{rating_value}
    {}

    //bool operator==(const RatingStar&) const = default;
    
    auto operator<=>(const RatingStar&) const = default;

    // auto operator<=>(const RatingValue& other) const
    // {
    //     return value <=> other;
    // }
};

auto operator<=>(RatingStar rs, RatingValue rv)
{
    return rs.value <=> rv;
}

TEST_CASE("Rating Star - implement <=>")
{
    RatingStar r1{RatingValue::good};
    
    CHECK(r1 == RatingStar{RatingValue::good});
    auto result = r1 <=> RatingStar{RatingValue::excellent};
    CHECK(result == std::strong_ordering::less);

    result = r1 <=> RatingValue::excellent;
    CHECK(result == std::strong_ordering::less);
}