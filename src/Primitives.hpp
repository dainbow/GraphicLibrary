#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>

#include "../Plugins/src/tools.hpp"

#include "Color.hpp"
#include "CordsPair.hpp"
#include "Vector.hpp"

const double TextScalar = 0.75;

class Rectangle {
    public:
        int64_t width_;
        int64_t height_;

        int64_t x_;
        int64_t y_;

        float rotation_ = 0;

        void Draw(sf::RenderTexture& widgetContainer, sf::Texture* texture) {
            sf::Vector2f sizesVec = {float(width_), float(height_)};
            sf::RectangleShape rect(sizesVec);

            rect.setPosition({float(x_), float(y_)});

            rect.setTexture(texture);
            rect.setRotation(float((rotation_ * 180.0) / M_PI));

            widgetContainer.draw(rect);
            widgetContainer.display();
        }
};

class Image : public booba::Image {
    private:
        sf::Image realImage_ = {};

    public:
        uint32_t width_  = 0;
        uint32_t height_ = 0;

        float rotation_ = 0;

        virtual ~Image() = default;

        virtual size_t getH() override {
            return height_;
        }

        virtual size_t getW() override {
            return width_;
        }

        void Copy(const Image& copy, uint32_t x, uint32_t y) {
            realImage_.copy(copy.realImage_, x, y);
        }

        virtual uint32_t getPixel(size_t x, size_t y) override {
            return GetPixel(uint32_t(x), uint32_t(y));
        }

        virtual void setPixel(size_t x, size_t y, uint32_t color) override {
            // std::cout << (color & 0xFF) << std::endl;
            SetPixel(uint32_t(x), uint32_t(y), color);
        }

        void SetPixel(uint32_t width, uint32_t height, const MyColor& color = {}) {
            MyColor curColor = (realImage_.getPixel(width, height).r << 24u) + (realImage_.getPixel(width, height).g << 16u) + (realImage_.getPixel(width, height).b << 8u) + realImage_.getPixel(width, height).a;
            MyColor nextColor = (color.red_ << 24u) + (color.green_ << 16u) + (color.blue_ << 8u) + color.alpha_;
            MyColor resultColor = curColor * (1.0 - double(nextColor.alpha_) / 255.0) + nextColor * (double(nextColor.alpha_) / 255.0);

            realImage_.setPixel(width, height, {resultColor.red_, resultColor.green_, resultColor.blue_});
        }

        MyColor GetPixel(uint32_t width, uint32_t height) {
            return MyColor((uint32_t(realImage_.getPixel(width, height).r) << 24) + (uint32_t(realImage_.getPixel(width, height).g) << 16) + (uint32_t(realImage_.getPixel(width, height).b) << 8) + 0xff);
        }

        bool LoadFromFile(const sf::String& imageName) {
            return realImage_.loadFromFile(imageName);
        }   

        Image(uint32_t width, uint32_t height, const MyColor& color = 0) {
            width_  = width;
            height_ = height;

            realImage_.create(width, height, {color.red_, color.green_, color.blue_});
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

        void Clear(const MyColor& color = 0) {
            for (uint32_t curX = 0; curX < width_; curX++) {
                for (uint32_t curY = 0; curY < height_; curY++) {
                    realImage_.setPixel(curX, curY, {color.red_, color.green_, color.blue_});
                }
            }
        }

        void Draw(sf::RenderTexture& container, const CordsPair& x0y0, const Vector& xyVirt, const uint32_t width, const uint32_t height) const {
            sf::Vector2f sizesVec = {float(width), float(height)};

            sf::RectangleShape rectangle(sizesVec);
            rectangle.setPosition({float(x0y0.x), float(x0y0.y)});

            sf::Texture curTexture = {};

            sf::IntRect area = {sf::Vector2i(int32_t(xyVirt.x_), int32_t(xyVirt.y_)), sf::Vector2i(width, height)};
            
            curTexture.loadFromImage(realImage_, area);

            rectangle.setTexture(&curTexture);

            rectangle.setRotation(float((rotation_ / M_PI) * 180.0));

            container.draw(rectangle);
            container.display();
        }
};

class Text {
    private:
        std::string realString_;
        MyColor textColor_;

    public: 
        float rotation_ = 0;

        uint64_t GetSize() {
            return realString_.size();
        }

        void SetText(const std::string& newString) {
            realString_ = newString;
        }

        Text(const std::string& newString = "", const MyColor& newColor = 0) :
        realString_(newString), textColor_(newColor) 
        {}

        std::string* GetRealString() {
            return &realString_;
        }

        void Draw(sf::RenderTexture& containter, const CordsPair& x0y0, [[maybe_unused]] const int64_t width, const int64_t height) {
            sf::Font font;
            font.loadFromFile("./fonts/arial.ttf");

            sf::Text text(realString_, font);
            text.setPosition({float(x0y0.x), float(x0y0.y)});

            text.setCharacterSize(uint32_t(double(height) * TextScalar));
            text.setStyle(sf::Text::Regular);

            sf::Color curColor(textColor_.red_, textColor_.green_, textColor_.blue_);
            text.setFillColor(curColor);

            text.setRotation(float((rotation_ / M_PI) * 180.0));

            containter.draw(text);
            containter.display();
        }
};  
