#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#include "Color.hpp"

class Window;

class Image {
    friend Window;

    private:
        sf::Image realImage_ = {};

    public:
        uint32_t width_  = 0;
        uint32_t height_ = 0;

        void SetPixel(uint32_t width, uint32_t height, const MyColor& color = {});

        bool LoadFromFile(const sf::String& imageName);
        
        void Create(uint32_t width, uint32_t height, const uint8_t* pixels);
        void Create(uint32_t width, uint32_t height, const MyColor& color = {});
};