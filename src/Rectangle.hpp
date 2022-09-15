#pragma once

#include "Vector.hpp"
#include "Color.hpp"

class Rectangle {
    public:
        Vector x0y0_;
        Vector x1y1_;

        MyColor color_;
        
        Rectangle(const Vector& x0y0, const Vector& x1y1, const MyColor& color = {}) : 
        x0y0_(x0y0), x1y1_(x1y1), color_(color) {};

    bool IsPressed(const Vector& curPoint) const;
};
