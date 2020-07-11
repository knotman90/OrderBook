#ifndef TRADE_H
#define TRADE_H

#include "Types.h"
#include <sstream>

struct Trade
{
    Trade() = default;
    Trade(const Client sell, const Client buy, const Price p, const Volume v) : seller(sell), buyer(buy), price(p), volume(v){}
    Client seller, buyer;
    Price price;
    Volume volume;

    bool operator==(const Trade&) const = default;
        
};

std::ostream& operator<<(std::ostream& out, const Trade& t){
    std::stringstream ss;
    ss<<"{";
    ss<<"buyer: "<< t.buyer<<", ";
    ss<<"seller: "<< t.seller<<", ";
    ss<<"price: "<< t.price<<", ";
    ss<<"volume: "<< t.volume;
    ss<<"}";
    out<<ss.str();
    return out;
}

#endif // TRADE_H