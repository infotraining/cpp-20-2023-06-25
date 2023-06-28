// Global module fragment
module;

#include <iostream>

// first thing after the Global module fragment must be a module command
export module foo;

export class Foo
{
public:
    Foo();
    ~Foo();
    void hello_world();
};

Foo::Foo() = default;
Foo::~Foo() = default;

void Foo::hello_world()
{
    std::cout << "Hello world!!!\n";
}