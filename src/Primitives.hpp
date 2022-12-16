#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <unordered_map>

#include "../Plugins/src/tools.hpp"

#include "Color.hpp"
#include "CordsPair.hpp"
#include "Vector.hpp"

const double TextScalar = 0.75;

const uint32_t MaxReturnAmount = 32;

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

class Image;

class Buffer {
    private:
        std::unordered_map<uint64_t, uint32_t> colors_ = {};
        std::unordered_map<uint64_t, std::pair<uint32_t, uint32_t>> buffer_[MaxReturnAmount] = {};
        std::unordered_map<uint64_t, std::pair<uint32_t, uint32_t>> localBuffer_ = {};

        uint32_t bufferHead_ = 0;
        uint32_t bufferTail_ = (MaxReturnAmount - 1);

        uint32_t savedHead_ = UINT32_MAX;

    public:
        void AddChange(const Change& change) {
            if (colors_.find((uint64_t(change.cords_.x) << 32) + uint64_t(change.cords_.y)) == colors_.end()) {
                colors_[(uint64_t(change.cords_.x) << 32) + uint64_t(change.cords_.y)] = change.oldColor_;
            }

            localBuffer_[(uint64_t(change.cords_.x) << 32) + uint64_t(change.cords_.y)] = {colors_[(uint64_t(change.cords_.x) << 32) + uint64_t(change.cords_.y)], change.newColor_};
        }

        void NextBuffer() {
            if (savedHead_ != UINT32_MAX) {
                for (uint32_t curHead = bufferHead_; ; curHead = ((curHead + 1) % MaxReturnAmount)) {
                    buffer_[curHead].clear();
                    
                    if (curHead == bufferTail_) {
                        break;
                    }
                }

                savedHead_ = UINT32_MAX;
            }

            buffer_[bufferHead_] = localBuffer_;
            localBuffer_.clear();

            bufferHead_ = (bufferHead_ + 1) % MaxReturnAmount;
            if (bufferHead_ == bufferTail_) {
                bufferTail_ = (bufferTail_ + 1) % MaxReturnAmount;
            }
        }

        void Undo(Image* image);
        void Nextdo(Image* image);
};

class Image : public booba::Image {
    private:
        sf::Image realImage_ = {};
        Buffer buffer_ = {};

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
            buffer_.AddChange({{x, y}, getPixel(uint32_t(x), uint32_t(y)), color});
            SetPixel(uint32_t(x), uint32_t(y), color);
        }

        void MoveBuffer() {
            buffer_.NextBuffer();
        }

        void UndoBuffer() {
            buffer_.Undo(this);
        }

        void NextdoBuffer() {
            buffer_.Nextdo(this);
        }

        void SetPixel(uint32_t width, uint32_t height, const MyColor& color = {}) {
            // MyColor curColor = (uint32_t(realImage_.getPixel(width, height).r) << 24u) + (uint32_t(realImage_.getPixel(width, height).g) << 16u) + (uint32_t(realImage_.getPixel(width, height).b) << 8u) + 0xff;
            // MyColor nextColor = (uint32_t(color.red_) << 24u) + (uint32_t(color.green_) << 16u) + (uint32_t(color.blue_) << 8u) + uint32_t(color.alpha_);
            // MyColor resultColor = curColor * (1.0 - nextColor.fAlpha_) + nextColor * nextColor.fAlpha_;

            // std::cout << (uint16_t)color.alpha_ << std::endl;
            realImage_.setPixel(width, height, {color.red_, color.green_, color.blue_, color.alpha_});
        }

        MyColor GetPixel(uint32_t width, uint32_t height) {
            return MyColor((uint32_t(realImage_.getPixel(width, height).r) << 24) + (uint32_t(realImage_.getPixel(width, height).g) << 16) + (uint32_t(realImage_.getPixel(width, height).b) << 8) + realImage_.getPixel(width, height).a);
        }

        bool LoadFromFile(const sf::String& imageName) {
            return realImage_.loadFromFile(imageName);
        }   

        Image(uint32_t width, uint32_t height, const MyColor& color = 0) {
            width_  = width;
            height_ = height;

            realImage_.create(width, height, {color.red_, color.green_, color.blue_, color.alpha_});
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
            sf::Uint32* pointer = reinterpret_cast<sf::Uint32*>(const_cast<sf::Uint8*>(realImage_.getPixelsPtr()));

            std::fill(pointer, pointer + realImage_.getSize().x * realImage_.getSize().y, color);
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
