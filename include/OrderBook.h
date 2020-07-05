#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <vector>
#include <set>

#include "Order.h"
#include "Types.h"
#include "Trade.h"

class OrderBook
{
    public:
        std::vector<Trade> submit(const Order& );
        void cancel(const Side side, const std::pair<Client, OrderId>& o);
        Volume get_volume_at_price(const Side side, const Price p);
        std::set<Price> get_prices(const Side side);
};

#endif // ORDERBOOK_H