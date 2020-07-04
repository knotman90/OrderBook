cmake_minimum_required(VERSION 2.8)

project(OrderBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(source)
add_subdirectory(include)
add_subdirectory(test)