set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")

# Default to C++ extensions being off. Clang's modules support have trouble
# with extensions right now and it is not required for any other compiler
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  include(gcc_modules.cmake)
endif()

set(CMAKE_CXX_STANDARD 23)

add_library(foo)

target_sources(foo
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    foo.cxx
)
add_executable(hello_modules main.cpp)
target_link_libraries(hello_modules PRIVATE foo)