#ifndef ORDER_H
#define ORDER_H

#include "Trade.h"
#include "Types.h"

struct Order {

  Client client;
  Product product;
  Side side;
  Price price;
  Volume volume;
  OrderId order_id;
};

bool operator==(const Order &o1, const Order &o2) {
  return (o1.client == o2.client) && (o1.product == o2.product) &&
         (o1.side == o2.side) && (o1.volume == o2.volume) &&
         (o1.order_id == o2.order_id);
}

#endif // ORDER_H