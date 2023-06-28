module;

#include <string>
#include <iostream>
#define NDEBUG
#include <cassert>

export module Foo;

export void foo(const std::string& s)
{
	assert(!s.empty());

	std::cout << "foo(" << s << ")\n";
}