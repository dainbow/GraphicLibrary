#pragma once

#include "Vector3d.hpp"
#include "Color.hpp"

struct LightSource {
    MyColor lightColor_;
    double intensivity_;

    LightSource(const MyColor& color = {0xffffffff}, const double number = 1) : lightColor_(color), intensivity_(number) {} 

    virtual ~LightSource() {};
};
