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
};

#endif // ORDER_H