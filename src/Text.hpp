#pragma once

#include <SFML/Graphics.hpp>
#include <cassert>

#include "Rectangle.hpp"
#include "Color.hpp"

const float textCoef = float(3) / float(4);

class Window;

class Text {
        friend Window;
    private:
        sf::Text realText_;
        sf::String fontPath_ = "./fonts/arial.ttf";
    public:
        sf::String textString_;
        Rectangle position_;

        Text();
        Text(const sf::String& string, const Rectangle& textRectangle, const MyColor& textColor = {});
};
