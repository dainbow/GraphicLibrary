#include "Image.hpp"

void Image::SetPixel(uint32_t width, uint32_t height, const MyColor& color) {
    realImage_.setPixel(width, height, {color.red_, color.green_, color.blue_});
}

bool Image::LoadFromFile(const sf::String& imageName) {
    return realImage_.loadFromFile(imageName);
}

void Image::Create(uint32_t width, uint32_t height, const uint8_t* pixels) {
    width_  = width;
    height_ = height;

    realImage_.create(width, height, pixels);
}

void Image::Create(uint32_t width, uint32_t height, const MyColor& color) {
    width_  = width;
    height_ = height;

    realImage_.create(width, height, {color.red_, color.green_, color.blue_});
}
