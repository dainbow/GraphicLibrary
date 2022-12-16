#pragma once 

#include <cstdint>

struct CordsPair {
    int32_t x;
    int32_t y;

    template <typename T>
    CordsPair(const T xS, const T yS) :
    x(int32_t(xS)), y(int32_t(yS))
    {}
};

struct Change {
    CordsPair cords_;

    uint32_t oldColor_;
    uint32_t newColor_;
};
