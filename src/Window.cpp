#include "Window.hpp"

#include <iostream>

bool Window::PollEvent(Event* curEvent) {
    sf::Event realEvent = {};
    bool returnValue = window_.pollEvent(realEvent);
    if (returnValue == 0)
        return 0;

    switch(realEvent.type) {
        case sf::Event::Closed:
            curEvent->type_ = Event::EventType::close;

            break;

        case sf::Event::MouseButtonPressed:
            curEvent->type_ = Event::EventType::mouseButtonPressed;

            curEvent->mouseCoords_.x_ = float(realEvent.mouseButton.x);
            curEvent->mouseCoords_.y_ = float(realEvent.mouseButton.y);
            break;

        default:
            break;
    }

    return 1;
}

void Window::DrawRectangle(const Rectangle& myRectangle) const {
    sf::RectangleShape rectangle(sf::Vector2f(myRectangle.x1y1_.x_ - myRectangle.x0y0_.x_, 
                                              myRectangle.x1y1_.y_ - myRectangle.x0y0_.y_));
    rectangle.setPosition({myRectangle.x0y0_.x_, myRectangle.x0y0_.y_});

    sf::Color curColor(myRectangle.color_.red_, myRectangle.color_.green_, myRectangle.color_.blue_);
    rectangle.setFillColor(curColor);

    (const_cast<sf::RenderWindow*>(&window_))->draw(rectangle);
}

void Window::DrawCoordSystem(const CoordSystem& curSystem) const {
    DrawRectangle({{curSystem.x0y0_.x_ - BorderWidth, curSystem.x0y0_.y_ - BorderWidth}, {curSystem.x1y1_.x_ + BorderWidth, curSystem.x1y1_.y_ + BorderWidth}, {0x9b, 0x2d, 0x30}});
    DrawRectangle({curSystem.x0y0_, curSystem.x1y1_, curSystem.color_});

    DrawVector(curSystem, {curSystem.xyMax_.x_ - curSystem.xyMin_.x_, 0}, {curSystem.xyMin_.x_, 0});
    DrawVector(curSystem, {0, curSystem.xyMax_.y_ - curSystem.xyMin_.y_}, {0, curSystem.xyMin_.y_});
}

void Window::DrawVector(const CoordSystem& curSystem, const Vector& curVector, const Vector& startPoint) const {
    DrawLine(curSystem, curVector, startPoint);

    DrawLine(curSystem, ( CreatePerpendicularVector(curVector) * PerpendCoef - curVector) * ArrowCoef , curVector + startPoint);
    DrawLine(curSystem, (-CreatePerpendicularVector(curVector) * PerpendCoef - curVector) * ArrowCoef, curVector + startPoint);
}

void Window::DrawLine(const CoordSystem& curSystem, const Vector& curVector, const Vector& startPoint) const {    
    if (curSystem.CheckPoint(startPoint) || curSystem.CheckPoint(startPoint + curVector))
        return;
    
    sf::Vertex line[] = {sf::Vertex(sf::Vector2f(curSystem.ConvertXToPxl(startPoint.x_), 
                            curSystem.ConvertYToPxl(startPoint.y_))),
                         sf::Vertex(sf::Vector2f(curSystem.ConvertXToPxl(startPoint.x_ + curVector.x_), 
                            curSystem.ConvertYToPxl(startPoint.y_ + curVector.y_)))};

    line[0].color = sf::Color::Black;
    line[1].color = sf::Color::Black;

    (const_cast<sf::RenderWindow*>(&window_))->draw(line, 2, sf::Lines);
}

void Window::DrawText(Text& text) const {
    sf::Font font;
    if (!font.loadFromFile(text.fontPath_))
    {
        assert(false && "CAN'T LOAD FONT");
    }
    text.realText_.setFont(font);

    (const_cast<sf::RenderWindow*>(&window_))->draw(text.realText_);
}

void Window::DrawButton(Button& button) const {
    DrawRectangle({button.x0y0_, button.x1y1_, button.color_});

    if (!CmpFloat(button.buttonText_.position_.x0y0_.x_, -1) ||
        !CmpFloat(button.buttonText_.position_.x0y0_.y_, -1)) {

        DrawText(button.buttonText_);
    }
} 

void Window::DrawImage(const Image& image, const Vector& x0y0) const {
    sf::RectangleShape rectangle(sf::Vector2f(float(image.width_), float(image.height_)));
    rectangle.setPosition({x0y0.x_, x0y0.y_});

    sf::Texture curTexture = {};
    curTexture.loadFromImage(image.realImage_);

    rectangle.setTexture(&curTexture);

    (const_cast<sf::RenderWindow*>(&window_))->draw(rectangle);
}

void Window::Resize(uint32_t width, uint32_t height) {
    window_.setSize({width, height});
}

void Window::Clear() {
    window_.clear();
}

void Window::Display() {
    window_.display();
}

Window::Window(uint32_t width, uint32_t height) :
        width_(width),
        height_(height),
        window_(sf::VideoMode(width, height), "Window") 
    {

    }

Window::~Window() {
    window_.close();
}
