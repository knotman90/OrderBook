#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <cassert>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "Order.h"
#include "Trade.h"
#include "Types.h"

template <typename T, typename K> struct hash_pair {
  size_t operator()(const std::pair<T, K> &p) const {
    return std::hash<T>{}(p.first) & std::hash<K>{}(p.second);
  }
};

class OrderBook {
private:
    void cancel_sell(const std::pair<Client, OrderId> &o);
    void cancel_buy(const std::pair<Client, OrderId> &o);
    std::vector<Trade> submit_buy_order(const Order &);
    std::vector<Trade> submit_sell_order(const Order &);
    
    Order& get_best_buy(){
        assert(buy_price_order_map.size() > 0);
        assert( (buy_price_order_map.rbegin()->second).size() > 0);
        return (buy_price_order_map.rbegin()->second).front();
    }

    Order& get_best_sell(){
        assert(sell_price_order_map.size() > 0);
        assert( (sell_price_order_map.begin()->second).size() > 0);
        return (sell_price_order_map.begin()->second).front();
    }

void add_to_price_volume_map(const Side& side, const std::pair<Price, Volume>& price_volume)
{
  auto [price, volume] = price_volume;
  if(side == Side::BUY)
  {
    buy_price_volume_map[price]+=volume;
    buy_prices.insert(price);
  }
  else{
    sell_price_volume_map[price]+=volume;
    sell_prices.insert(price);
  }
}

public:
  std::vector<Trade> submit(const Order &);
  void cancel(const Side side, const std::pair<Client, OrderId> &o);
  Volume get_volume_at_price(const Side side, const Price p);
  std::set<Price> get_prices(const Side side);

  using OrderMap = std::map<Price, std::list<Order>>;
  using OrderMapIt = typename OrderMap::mapped_type::iterator;
  OrderMap buy_price_order_map;
  OrderMap sell_price_order_map;
  std::unordered_map<std::pair<Client, OrderId>, OrderMapIt,
                     hash_pair<Client, OrderId>>
      client_order_map;

  using PriceVolumeMap = std::unordered_map<Price, Volume>;
  PriceVolumeMap sell_price_volume_map;
  PriceVolumeMap buy_price_volume_map;

  std::set<Price> sell_prices;
  std::set<Price> buy_prices;
};

#endif // ORDERBOOK_H