#ifndef TYPES_H
#define TYPES_H

#include <string>


enum class Side { BUY, SELL };

Side get_opposite_side(const Side &side) {
  if (side == Side::BUY)
    return Side::SELL;
  return Side::BUY;
}


using OrderId = size_t;
using Client = size_t;
using Volume = int;
using Price = int;
using Product = std::string;

#endif // TYPES_H