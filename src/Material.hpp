#pragma once

#include <cstdint>

#include "Color.hpp"

struct Material {
    double reflectPowerDecrease_    = 0;
    double refractPowerDecrease_    = 0;
    
    double materialRefractionCoef_   = 0;

    uint32_t reflectibility_ = 0;
    bool isTransparent_ = 0;

    virtual ~Material() {};
};