#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>

#include "Vector.hpp"
#include "CoordSystem.hpp"
#include "Event.hpp"
#include "Rectangle.hpp"
#include "Button.hpp"
#include "Image.hpp"

class Window {
    private:
        uint32_t width_;
        uint32_t height_;
        
        sf::RenderWindow window_;


    public:

    bool IsOpen() const {
        return window_.isOpen();
    }

    bool PollEvent(Event* curEvent);

    void DrawRectangle(const Rectangle& rectangle) const;
    void DrawCoordSystem(const CoordSystem& curSystem) const;
    void DrawLine(const CoordSystem& curSystem, const Vector& curVector, const Vector& startPoint = {0, 0}) const;
    void DrawVector(const CoordSystem& curSystem, const Vector& curVector, const Vector& startPoint = {0, 0}) const;
    void DrawText(Text& text) const;
    void DrawButton(Button& button) const;
    void DrawImage(const Image& image, const Vector& x0y0) const;

    void Resize(uint32_t width, uint32_t height);
    void Clear();
    void Display();

    Window(uint32_t width, uint32_t height);
    ~Window();
};
