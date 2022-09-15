#pragma once

#include "Rectangle.hpp"
#include "Text.hpp"

class Button : public Rectangle {
    public:
        Text buttonText_;

        Button(const Vector& x0y0, const Vector& x1y1, const MyColor& color = {}, sf::String string = {}, const MyColor& textColor = {}):
        Rectangle(x0y0, x1y1, color), buttonText_(string, {x0y0, x1y1}, textColor) {}
};
