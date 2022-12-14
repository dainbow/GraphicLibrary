#pragma once

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

enum class SkinIdxs {
    NoSkin = 0,
    RealWindowBackground = 1,
    WindowBackground     = 2,
    ButtonNotPressed     = 3,
    ButtonPressed        = 4,
    ButtonHovering       = 5,
    ScrollUp             = 6,
    ScrollDown           = 7,
    ScrollMiddle         = 8,
    ScrollBackground     = 9,
    BarBackground        = 10,
    BarProgress          = 11,
    InactiveText         = 12,
    ActiveText           = 13,

    COUNT,
}; 

class SkinManager {
    private:
        sf::Texture* textures_;

    public:
        SkinManager(const SkinManager& manager)            = delete;
        SkinManager& operator=(const SkinManager& manager) = delete;

        SkinManager(const std::string& skinName) :
        textures_(nullptr) 
        {
            std::stringstream skinNameStream;
            skinNameStream << "./Skins/" << skinName << "/";
            
            textures_ = new sf::Texture[static_cast<int32_t>(SkinIdxs::COUNT)]();
            for (uint32_t curSkin = 0; curSkin < static_cast<int32_t>(SkinIdxs::COUNT); curSkin++) {
                if(!textures_[curSkin].loadFromFile(skinNameStream.str() + std::to_string(curSkin) + ".png")) {
                    textures_[curSkin].loadFromFile("./Skins/empty.png");
                }
            }
        }

        ~SkinManager() {
            delete[] textures_;
        }

        static const SkinManager& GetInstance(const std::string& skinName) {
            static SkinManager instance(skinName);

            return instance;
        }

        sf::Texture* GetTexture(const SkinIdxs& skinNumber) const {
            return textures_ + static_cast<int32_t>(skinNumber);
        }
};