module;

#include <iostream>

export module FooMod;

export namespace FooMod
{
    void foo(); // FooMod::foo()
}

void FooMod::foo()
{
    std::cout << "Hello modules\n";
}