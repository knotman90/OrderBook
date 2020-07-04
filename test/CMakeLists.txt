cmake_minimum_required(VERSION 3.9)

include(External_GTest.cmake)


add_executable(test_OrderBook test_OrderBook.cpp)

message (${GTEST_LIBRARY})
target_link_libraries(test_OrderBook ${GTEST_LIBRARY})
target_include_directories(test_OrderBook
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

# Tell ctest about my tests
include(GoogleTest)
gtest_add_tests(
    TARGET      test_OrderBook
    TEST_LIST   test_OrderBook_tgt
)