cmake_minimum_required(VERSION 3.9)

project(OrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb3")

enable_testing()

add_subdirectory(source)
add_subdirectory(include)
add_subdirectory(test)