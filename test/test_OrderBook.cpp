#include "gtest/gtest.h"

#include "OrderBook.h"

class OrderBook_Fixture : public ::testing::Test{
    protected:
        OrderBook ob;

};

TEST_F(OrderBook_Fixture, when_empty_book_no_trades_generated)
{
    
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}