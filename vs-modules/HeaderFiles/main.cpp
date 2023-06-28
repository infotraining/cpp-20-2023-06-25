#include <cassert>

import string;
import Foo;

int main()
{
	std::string str = "";
	foo(str); // assert in foo will not fire

	assert(!str.empty()); // this assert will fire
}