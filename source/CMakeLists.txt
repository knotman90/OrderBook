add_library(libbook SHARED
    OrderBook.cpp
)

target_include_directories(libbook
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)