#include "gtest/gtest.h"

#include "Order.h"
#include "OrderBook.h"


Order create_order(Client c, const Side &s, const Product &product,
                   const Price &price, const Volume &volume) {
 static OrderId next_order_id = 0;
 
  Order o;
  o.client = c;
  o.side = s;
  o.product = product;
  o.price = price;
  o.volume = volume;
  o.order_id = next_order_id++;
  return o;
}

class OrderBook_Fixture : public ::testing::Test {
protected:
  OrderBook ob;
  Client clientID = 0;

  Client get_next_clientID() { return clientID++; }

  Client get_clientID(const int n = 0) { return n; }
};

TEST_F(OrderBook_Fixture,
       given_an_empty_book_no_trades_generated_when_BUY_order_is_submitted) {
  constexpr Price price = 1000;
  constexpr Volume volume = 123;
  constexpr Side side = Side::BUY;
  const Client client = get_next_clientID();
  const Order o = create_order(client, side, "DUMMY", price, volume);
  const auto &traded = ob.submit(o);

  ASSERT_EQ(0, traded.size());
  ASSERT_EQ(volume, ob.get_volume_at_price(side, price));
  ASSERT_EQ(0, ob.get_volume_at_price(get_opposite_side(side), price));

  const std::set<Price> expected_prices = {price};
  ASSERT_EQ(expected_prices, ob.get_prices(side));
  ASSERT_EQ(0, ob.get_prices(get_opposite_side(side)).size());
}

TEST_F(OrderBook_Fixture,
       given_an_empty_book_no_trades_generated_when_SELL_order_is_submitted) {
  constexpr Price price = 1000;
  constexpr Volume volume = 123;
  constexpr Side side = Side::SELL;
  const Client client = get_next_clientID();
  const Order o = create_order(client, side, "DUMMY", price, volume);

  const auto &traded = ob.submit(o);
  ASSERT_EQ(0, traded.size());
  ASSERT_EQ(volume, ob.get_volume_at_price(side, price));
  ASSERT_EQ(0, ob.get_volume_at_price(get_opposite_side(side), price));

  const std::set<Price> expected_prices = {price};
  ASSERT_EQ(expected_prices, ob.get_prices(side));
  ASSERT_EQ(0, ob.get_prices(get_opposite_side(side)).size());
}

TEST_F(
    OrderBook_Fixture,
    given_an_empty_book_no_trades_generated_when_only_SELL_orders_are_submitted) {
  const Client client = get_next_clientID();
  for (int i = 0; i < 100; i++) {
    const Order o = create_order(client, Side::SELL, "DUMMY", 1000 * i, 1 + i);
    const auto &traded = ob.submit(o);
    ASSERT_EQ(0, traded.size());

    ASSERT_EQ(1 + i, ob.get_volume_at_price(Side::SELL, 1000 * i));
  }
}

TEST_F(
    OrderBook_Fixture,
    given_an_empty_book_no_trades_generated_when_only_BUY_for_different_prices_orders_are_submitted) {
  const Client client = get_next_clientID();
  for (int i = 0; i < 100; i++) {
    const Volume volume = 1 ;
    const Price price = 1000 + i;
    const Order o = create_order(client, Side::BUY, "DUMMY", 1000 + i, 1);
    const auto &traded = ob.submit(o);
    ASSERT_EQ(0, traded.size());

    ASSERT_EQ(volume, ob.get_volume_at_price(Side::BUY, price));
  }
}


TEST_F(
    OrderBook_Fixture,
    given_an_empty_book_no_trades_generated_when_only_BUY_for_same_prices_orders_are_submitted) {
  const Client client = get_next_clientID();
  for (int i = 0; i < 100; i++) {
    const Volume volume = 1 +i;
    const Price price = 1000 ;
    const Order o = create_order(client, Side::BUY, "DUMMY", 1000, 1);
    const auto &traded = ob.submit(o);
    ASSERT_EQ(0, traded.size());

    ASSERT_EQ(volume, ob.get_volume_at_price(Side::BUY, price));

  }
}

TEST_F(
    OrderBook_Fixture, given_a_book_with_only_buy_orders_when_a_sell_order_arrives_price_of_trade_is_passive) {
  const Client client = get_next_clientID();

  std::set<Price> expected_buy_prices = {}; 
  std::set<Price> expected_sell_prices = {};
  ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
  ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  for (int i = 0; i < 100; i++) {
    const Volume volume = 1 + i;
    const Price price = 1000 +i;
    const Order o = create_order(client, Side::BUY, "DUMMY", price, volume);
    const auto &traded = ob.submit(o);
    ASSERT_EQ(0, traded.size());

    ASSERT_EQ(volume, ob.get_volume_at_price(Side::BUY, price));

    expected_buy_prices.insert(price);
    ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));    
    ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  }
  const Client next_client = get_next_clientID();
  const Order large_sell_order = create_order(next_client, Side::SELL, "DUMMY", 10, 100000);
  const auto& trades = ob.submit(large_sell_order);
  ASSERT_EQ(100, trades.size());
  for(int i = 0 ; i < 100 ; i++){
    Trade expected_trade;
    expected_trade.buyer = client;
    expected_trade.seller = next_client;
    expected_trade.price = 1000+i;
    expected_trade.volume = 1+i; 
    ASSERT_TRUE(std::find(begin(trades), end(trades), expected_trade) != trades.end());
  }
}

TEST_F(
    OrderBook_Fixture, given_a_book_with_only_sell_orders_when_a_sell_order_arrives_price_of_trade_is_passive) {
  const Client client = get_next_clientID();

  std::set<Price> expected_buy_prices = {}; 
  std::set<Price> expected_sell_prices = {};
  ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
  ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));

  for (int i = 0; i < 100; i++) {
    const Volume volume = 1 + i;
    const Price price = 1000 +i;
    const Order o = create_order(client, Side::SELL, "DUMMY", price, volume);
    const auto &traded = ob.submit(o);
    ASSERT_EQ(0, traded.size());

    ASSERT_EQ(volume, ob.get_volume_at_price(Side::SELL, price));

    expected_sell_prices.insert(price);
    ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));    
    ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  }

  const Client next_client = get_next_clientID();
  const Order large_buy_order = create_order(next_client, Side::BUY, "DUMMY", 100000, 100000);
  const auto& trades = ob.submit(large_buy_order);
  ASSERT_EQ(100, trades.size());
  for(int i = 0 ; i < 100 ; i++){
    Trade expected_trade;
    expected_trade.buyer = next_client;
    expected_trade.seller = client;
    expected_trade.price = 1000+i;
    expected_trade.volume = 1+i; 
    ASSERT_TRUE(std::find(begin(trades), end(trades), expected_trade) != trades.end());
  }
  expected_buy_prices.clear();
  expected_buy_prices.insert(large_buy_order.price);
  
  ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
  expected_sell_prices.clear();
  ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
}


TEST_F(OrderBook_Fixture, from_handout) {
  const Client Alice = get_next_clientID();
  const Client Bob = get_next_clientID();
  const Client Carol = get_next_clientID();
  const Client Dave = get_next_clientID();
  const Client Erin = get_next_clientID();
  // Alice buys 100 shares at 150
  {
    Order order_alice = create_order(Alice, Side::BUY, "ABC", 150, 100);
    ob.submit(order_alice);

    std::set<Price> expected_buy_prices = {150}; 
    ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
    
    std::set<Price> expected_sell_prices = {}; 
    ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  }

  // Bob buys 100 shares at 160
  {
    Order order_bob = create_order(Bob, Side::BUY, "ABC", 160, 100);
    const auto &trades = ob.submit(order_bob);
    ASSERT_TRUE(trades.empty()); // no trades
    // check volumes for all prices from 0 to 10000
    for (int i = 0; i < 10000; i++) {
      if ((i != 150) && (i != 160)) {
        ASSERT_EQ(0, ob.get_volume_at_price(Side::BUY, i));
      }
    }
    ASSERT_EQ(100, ob.get_volume_at_price(Side::BUY, 150));
    ASSERT_EQ(100, ob.get_volume_at_price(Side::BUY, 160));

    std::set<Price> expected_buy_prices = {160, 150}; 
    ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
    
    std::set<Price> expected_sell_prices = {}; 
    ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  }

  // Carol buys 100 shares at 160
  {
    Order order_carol = create_order(Carol, Side::BUY, "ABC", 160, 100);
    const auto &trades = ob.submit(order_carol);
    ASSERT_TRUE(trades.empty()); // no trades
    ASSERT_EQ(100, ob.get_volume_at_price(Side::BUY, 150));
    ASSERT_EQ(200, ob.get_volume_at_price(Side::BUY,
                                          160)); // 200 shares at 160. Bob+Carol

    std::set<Price> expected_buy_prices = {160, 150}; 
    ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
    
    std::set<Price> expected_sell_prices = {}; 
    ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  }

  // Dave sells 150 shares at 160$
  {
    Order order_carol = create_order(Dave, Side::SELL, "ABC", 160, 150);
    const auto &trades = ob.submit(order_carol);
    ASSERT_EQ(2, trades.size());

    const Trade expected_trade1 = Trade(Dave, Bob, 160, 100);
    ASSERT_EQ(expected_trade1, trades[0]);
    const Trade expected_trade2 = Trade(Dave, Carol, 160, 50);
    ASSERT_EQ(expected_trade2, trades[1]);

    ASSERT_EQ(100, ob.get_volume_at_price(Side::BUY, 150));
    ASSERT_EQ(50, ob.get_volume_at_price(
                      Side::BUY,
                      160)); // 50 shares left at 160 from Carol's order

    std::set<Price> expected_buy_prices = {160, 150}; 
    ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
    
    std::set<Price> expected_sell_prices = {}; 
    ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  }

  // Erin sells 100 shares at 150$
  {
    Order order_carol = create_order(Erin, Side::SELL, "ABC", 150, 100);
    const auto &trades = ob.submit(order_carol);
    ASSERT_EQ(2, trades.size());

    const Trade expected_trade1 = Trade(Erin, Carol, 160, 50);
    ASSERT_EQ(expected_trade1, trades[0]);
    const Trade expected_trade2 = Trade(Erin, Alice, 150, 50);
    ASSERT_EQ(expected_trade2, trades[1]);

    ASSERT_EQ(50, ob.get_volume_at_price(Side::BUY,
                                         150)); // 50 buy shares from Alice left
    ASSERT_EQ(0, ob.get_volume_at_price(Side::BUY,
                                        160)); // no more buy orders at 160
    
    std::set<Price> expected_buy_prices = {150}; 
    ASSERT_EQ(expected_buy_prices, ob.get_prices(Side::BUY));
    
    std::set<Price> expected_sell_prices = {}; 
    ASSERT_EQ(expected_sell_prices, ob.get_prices(Side::SELL));
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}