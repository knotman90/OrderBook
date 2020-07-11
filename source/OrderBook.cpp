#include "OrderBook.h"

std::vector<Trade> OrderBook::submit(const Order &order) {
  if (order.side == Side::BUY) {
    return submit_buy_order(order);
  } else {
    return submit_sell_order(order);
  }
}

std::vector<Trade> OrderBook::submit_buy_order(const Order &order) {
  std::vector<Trade> ans;
  Volume remaining_volume = order.volume;

  while (sell_price_order_map.size() > 0 &&
         get_best_sell().price <= order.price && remaining_volume > 0) {
    auto &passive = get_best_sell();
    Trade t;
    t.seller = passive.client;
    t.buyer = order.client;
    t.price = passive.price;
    const Volume traded_volume = std::min(remaining_volume, passive.volume);
    t.volume = traded_volume;

    remaining_volume -= traded_volume;
    ans.push_back(t);

    if (traded_volume >= passive.volume) {
      cancel(Side::SELL, {passive.client, passive.order_id});
    } else {
      sell_price_volume_map[passive.price] -= traded_volume;
      passive.volume -= traded_volume;
    }
  }

  if (remaining_volume > 0) {
    Order new_order(order);
    new_order.volume = remaining_volume;
    auto &list = buy_price_order_map[new_order.price];
    auto it = list.insert(std::end(list), new_order);
    client_order_map[{order.client, order.order_id}] = it;

    add_to_price_volume_map(Side::BUY, {new_order.price, new_order.volume});
  }

  return ans;
}

std::vector<Trade> OrderBook::submit_sell_order(const Order &order) {
  std::vector<Trade> ans;
  Volume remaining_volume = order.volume;
  while (buy_price_order_map.size() > 0 &&
         get_best_buy().price >= order.price && remaining_volume > 0) {
    auto &passive = get_best_buy();
    Trade t;
    t.buyer = passive.client;
    t.seller = order.client;
    t.price = passive.price;
    const Volume traded_volume = std::min(remaining_volume, passive.volume);
    t.volume = traded_volume;

    remaining_volume -= traded_volume;
    ans.push_back(t);

    if (traded_volume >= passive.volume) {
      cancel(Side::BUY, {passive.client, passive.order_id});
    } else {
      buy_price_volume_map[passive.price] -= traded_volume;
      passive.volume -= traded_volume;
    }
  }
  if (remaining_volume > 0) {
    Order new_order(order);
    new_order.volume = remaining_volume;
    auto &list = sell_price_order_map[new_order.price];
    auto it = list.insert(std::end(list), new_order);
    client_order_map[{order.client, order.order_id}] = it;

    add_to_price_volume_map(Side::SELL, {new_order.price, new_order.volume});
  }

  return ans;
}

Volume OrderBook::get_volume_at_price(const Side side, const Price p) {
  if (side == Side::BUY) {
    if (buy_price_volume_map.contains(p))
      return buy_price_volume_map.find(p)->second;
    return 0;
  }
  if (sell_price_volume_map.contains(p))
    return sell_price_volume_map.find(p)->second;
  return 0;
}

std::set<Price> OrderBook::get_prices(const Side side) {
  if (side == Side::BUY)
    return buy_prices;
  return sell_prices;
}

void OrderBook::cancel(const Side side, const std::pair<Client, OrderId> &o) {
  assert(client_order_map.contains(o));
  if (side == Side::BUY)
    cancel_buy(o);
  else
    cancel_sell(o);
}

void OrderBook::cancel_buy(const std::pair<Client, OrderId> &o) {
  OrderMapIt it = client_order_map[o];
  const Order to_be_canceled = *(it);
  {
    // ASSERTIONS
    assert(buy_price_order_map.contains(to_be_canceled.price));
    assert(buy_price_order_map[to_be_canceled.price].size() > 0);
    assert(std::find(buy_price_order_map[to_be_canceled.price].begin(),
                     buy_price_order_map[to_be_canceled.price].end(),
                     to_be_canceled) !=
           buy_price_order_map[to_be_canceled.price].end());
    assert(buy_price_volume_map.contains(to_be_canceled.price));
    assert(buy_price_volume_map[to_be_canceled.price] >= to_be_canceled.volume);
  }

  auto& list = buy_price_order_map[to_be_canceled.price];
  list.erase(it);
  if(list.empty())
    buy_price_order_map.erase(to_be_canceled.price);
  auto volume_at_price_it = buy_price_volume_map.find(to_be_canceled.price);
  Volume &volume_at_price = volume_at_price_it->second;
  volume_at_price -= to_be_canceled.volume;
  if (volume_at_price == 0) {
    buy_price_volume_map.erase(volume_at_price_it);
    buy_prices.erase(to_be_canceled.price);
  }
}

void OrderBook::cancel_sell(const std::pair<Client, OrderId> &o) {
  OrderMapIt it = client_order_map[o];
  const Order to_be_canceled = *(it);
  {
    // ASSERTIONS
    assert(sell_price_order_map.contains(to_be_canceled.price));
    assert(sell_price_order_map[to_be_canceled.price].size() > 0);
    assert(std::find(sell_price_order_map[to_be_canceled.price].begin(),
                     sell_price_order_map[to_be_canceled.price].end(),
                     to_be_canceled) !=
           sell_price_order_map[to_be_canceled.price].end());
    assert(sell_price_volume_map.contains(to_be_canceled.price));
    assert(sell_price_volume_map[to_be_canceled.price] >=
           to_be_canceled.volume);
  }
   auto& list = sell_price_order_map[to_be_canceled.price];
  list.erase(it);
  if(list.empty())
    sell_price_order_map.erase(to_be_canceled.price);
  
  auto volume_at_price_it = sell_price_volume_map.find(to_be_canceled.price);
  Volume &volume_at_price = volume_at_price_it->second;
  volume_at_price -= to_be_canceled.volume;
  if (volume_at_price == 0) {
    sell_price_volume_map.erase(volume_at_price_it);
    sell_prices.erase(to_be_canceled.price);
  }
}