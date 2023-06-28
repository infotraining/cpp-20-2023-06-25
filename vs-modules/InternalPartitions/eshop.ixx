module; // start module unit with global module fragment

#include <iostream>
#include <string>
#include <vector>

export module EShop;  // module declaration

import :Order;  // import internal partition Order

export class Customer
{
private:
    std::string name_;
    std::vector<Order> orders_;

public:
    Customer(std::string name)
        : name_{std::move(name)}
    {}

    void buy(std::string order_name, double price)
    {
        std::cout << name_ << " is buying " << order_name << " for a " << price << "$\n";
        orders_.push_back(Order{1, std::move(order_name), price});
    }

    void buy(uint32_t count, std::string order_name, double price)
    {
        orders_.push_back(Order{count, std::move(order_name), price});
    }

    double total_price() const;
    double average_price() const;
    void print() const;
};