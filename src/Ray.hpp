#pragma once

#include "Vector3d.hpp"
#include "Color.hpp"

class Ray {
    public:
        Vector3D point_;
        Vector3D vector_;

        double power_;

        bool IsNan() const {
            return point_.IsNan() && vector_.IsNan() && std::isnan(power_);
        }
};
