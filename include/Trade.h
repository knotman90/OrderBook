#ifndef TRADE_H
#define TRADE_H

#include "Types.h"

struct Trade
{
    Trade() = default;
    Trade(const Client sell, const Client buy, const Price p, const Volume v) : seller(sell), buyer(buy), price(p), volume(v){}
    Client seller, buyer;
    Price price;
    Volume volume;

    bool operator==(const Trade&) const = default;
        
};

#endif // TRADE_H