#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <span>

using namespace std::literals;

void print_items(std::span<const int> data)
{
    for (const auto& item : data)
        std::cout << item << " ";
    std::cout << "\n";
}

void zero(const std::span<int> data, int zero_value = 0)
{
    for(auto& item : data)
        item = zero_value;
}

TEST_CASE("std::span")
{
    SECTION("fixed extent")
    {
        int raw_data[16] = {1, 2, 3, 4};

        std::span<int, 16> all_data(raw_data, 16);

        static_assert(all_data.size());

        print_items(all_data);
    }

    SECTION("dynamic extent")
    {
        std::vector vec = {1, 2, 3, 4, 5, 6, 7, 8};

        std::span<int> all_data(vec);

        CHECK(all_data.size() == 8);

        print_items(all_data);

        zero(std::span{vec.begin(), 4});

        zero(all_data.last(4), 1);

        print_items(all_data);
    }
}

void print_as_bytes(const float f, const std::span<const std::byte> bytes)
{
	std::cout << std::format("{:+6}", f) << " - { ";

	for(const std::byte b : bytes)
	{
		std::cout << std::format("{:02X} ", std::to_integer<int>(b));
	}

	std::cout << "}\n";
}

TEST_CASE("span of bytes")
{
	float data[] = { 3.141592f };

	auto const_bytes = std::as_bytes(std::span{data});
	print_as_bytes(data[0], const_bytes);

	auto const writable_bytes = std::as_writable_bytes(std::span{data});
	writable_bytes[3] |= std::byte{ 0b1000'0000 };
	print_as_bytes(data[0], writable_bytes);
}
