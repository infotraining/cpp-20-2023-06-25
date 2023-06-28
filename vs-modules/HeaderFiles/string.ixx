// This named module expects to be built with classic headers, not header units.
#define _BUILD_STD_MODULE

module;

export module string;

#pragma warning(push)
#pragma warning(disable : 5244) // '#include <meow>' in the purview of module 'std' appears erroneous.

#include <string>

#pragma warning(pop)