#pragma once

#include <SFML/Graphics.hpp>

#include "Vector.hpp"

class Window;

class Event {
    friend Window;

    private:
        sf::Event realEvent_ = {};

    public:
        enum class EventType {
            nothing,
            close,
            mouseButtonPressed
        };

        EventType type_     = EventType::nothing;
        Vector mouseCoords_ = {};
};  
