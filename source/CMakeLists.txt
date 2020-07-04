add_library(OrderBook SHARED
    OrderBook.cpp
)

target_include_directories(OrderBook
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)