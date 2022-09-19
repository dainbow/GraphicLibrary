#pragma once

#include "Vector3d.hpp"
#include "Color.hpp"

class LightSource {
    public:
        Vector3D point_;

        MyColor color_;
        double intensivity_;
};
