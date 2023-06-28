module; // start module unit with global module fragment

#include <iostream>

#ifdef __has_cpp_attribute(__cpp_lib_format)
#include <format>
#endif

module EShop;

void Customer::print() const
{
    std::cout << name_ << ":\n";

    for (const auto& order : orders_)
    {
#ifdef __has_cpp_attribute(__cpp_lib_format)
        std::cout <<
            std::format("{:3} {:14} {:6.2f} {:6.2f}\n",
                order.count, order.name, order.price, order.count * order.price);
#else
        std::cout << order.count
            << " - " << order.name
            << " - " << order.price << "$\n";
#endif        
    }

    std::cout << "------------------------\n";
    std::cout << "Total price: " << total_price() << "\n";
}
