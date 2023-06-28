module;  // start module unit with global module fragment

#include <string>

module EShop:Order; // internal partition declaration

struct Order
{
    uint32_t count;
    std::string name;
    double price;

    Order(uint32_t count, std::string order_name, double price)
        : count{count}
        , name{std::move(order_name)}
        , price{price}
    {}
};