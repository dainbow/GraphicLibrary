#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>

#include "Color.hpp"

const double TextScalar = 0.75;

class Rectangle {
    public:
        int64_t width_;
        int64_t height_;

        int64_t x_;
        int64_t y_;

        // Rectangle (uint32_t width, uint32_t height, uint32_t x, uint32_t y) :
        // width_(width), height_(height),
        // x_(x), y_(y)
        // {}

        void Draw(sf::RenderWindow* window, const MyColor& color) {
            assert(window);

            sf::Vector2f sizesVec = {float(width_), float(height_)};
            sf::RectangleShape rect(sizesVec);

            rect.setPosition({float(x_), float(y_)});

            sf::Color curColor(color.red_, color.green_, color.blue_);
            rect.setFillColor(curColor);

            window->draw(rect);
        }
};

class Image {
    private:
        sf::Image realImage_ = {};

    public:
        uint32_t width_  = 0;
        uint32_t height_ = 0;

        void SetPixel(uint32_t width, uint32_t height, const MyColor& color = {}) {
            realImage_.setPixel(width, height, {color.red_, color.green_, color.blue_});
        }

        bool LoadFromFile(const sf::String& imageName) {
            return realImage_.loadFromFile(imageName);
        }   
        
        void Create(uint32_t width, uint32_t height, const uint8_t* pixels) {
            width_  = width;
            height_ = height;

            realImage_.create(width, height, pixels);
        }
        
        void Create(uint32_t width, uint32_t height, const MyColor& color = {}) {
            width_  = width;
            height_ = height;

            realImage_.create(width, height, {color.red_, color.green_, color.blue_});
        }

        void Draw(sf::RenderWindow* window, const Vector& x0y0) const {
            assert(window);

            sf::Vector2f sizesVec = {float(width_), float(height_)};

            sf::RectangleShape rectangle(sizesVec);
            rectangle.setPosition({float(x0y0.x_), float(x0y0.y_)});

            sf::Texture curTexture = {};
            curTexture.loadFromImage(realImage_);

            rectangle.setTexture(&curTexture);

            window->draw(rectangle);
        }
};

class Text {
    private:
        sf::String realString_;
        MyColor textColor_;

    public: 
        uint64_t GetSize() {
            return realString_.getSize();
        }

        void SetText(const sf::String& newString) {
            realString_ = newString;
        }

        Text(const sf::String& newString = "", const MyColor& newColor = 0) :
        realString_(newString), textColor_(newColor) 
        {}

        void Draw(sf::RenderWindow* window, const Vector& x0y0, [[maybe_unused]] const int64_t width, const int64_t height) {
            assert(window);
            
            sf::Font font;
            font.loadFromFile("./fonts/arial.ttf");

            sf::Text text(realString_, font);
            text.setPosition({float(x0y0.x_), float(x0y0.y_)});

            text.setCharacterSize(uint32_t(double(height) * TextScalar));
            text.setStyle(sf::Text::Regular);

            sf::Color curColor(textColor_.red_, textColor_.green_, textColor_.blue_);
            text.setFillColor(curColor);

            window->draw(text);
        }
};  
