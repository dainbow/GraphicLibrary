#include "Window.hpp"

#include <iostream>

RealWindow::RealWindow(uint32_t width, uint32_t height, const MyColor& color) :
Window(0, 0, width, height, color),
realWindow_(sf::VideoMode(width, height), "Window"),
isPressed_(0)
{   
    ptrToRealWdw_ = &realWindow_;
};



// void Window::DrawCoordSystem(const CoordSystem& curSystem) const {
//     DrawRectangle({{curSystem.x0y0_.x_ - BorderWidth, curSystem.x0y0_.y_ - BorderWidth}, {curSystem.x1y1_.x_ + BorderWidth, curSystem.x1y1_.y_ + BorderWidth}, {0x9b, 0x2d, 0x30}});
//     DrawRectangle({curSystem.x0y0_, curSystem.x1y1_, curSystem.color_});

//     DrawVector(curSystem, {curSystem.xyMax_.x_ - curSystem.xyMin_.x_, 0}, {curSystem.xyMin_.x_, 0});
//     DrawVector(curSystem, {0, curSystem.xyMax_.y_ - curSystem.xyMin_.y_}, {0, curSystem.xyMin_.y_});
// }

// void Window::DrawVector(const CoordSystem& curSystem, const Vector& curVector, const Vector& startPoint) const {
//     DrawLine(curSystem, curVector, startPoint);

//     DrawLine(curSystem, ( CreatePerpendicularVector(curVector) * PerpendCoef - curVector) * ArrowCoef , curVector + startPoint);
//     DrawLine(curSystem, (-CreatePerpendicularVector(curVector) * PerpendCoef - curVector) * ArrowCoef, curVector + startPoint);
// }

// void Window::DrawLine(const CoordSystem& curSystem, const Vector& curVector, const Vector& startPoint) const {    
//     if (curSystem.CheckPoint(startPoint) || curSystem.CheckPoint(startPoint + curVector))
//         return;
    
//     sf::Vertex line[] = {sf::Vertex(sf::Vector2f(float(curSystem.ConvertXToPxl(startPoint.x_)), 
//                             float(curSystem.ConvertYToPxl(startPoint.y_)))),
//                          sf::Vertex(sf::Vector2f(float(curSystem.ConvertXToPxl(startPoint.x_ + curVector.x_)), 
//                             float(curSystem.ConvertYToPxl(startPoint.y_ + curVector.y_))))};

//     line[0].color = sf::Color::Black;
//     line[1].color = sf::Color::Black;

//     (const_cast<sf::RenderWindow*>(&window_))->draw(line, 2, sf::Lines);
// }

// void Window::DrawText(Text& text) const {
//     sf::Font font;
//     if (!font.loadFromFile(text.fontPath_))
//     {
//         assert(false && "CAN'T LOAD FONT");
//     }
//     text.realText_.setFont(font);

//     (const_cast<sf::RenderWindow*>(&window_))->draw(text.realText_);
// }

// void Window::DrawButton(Button& button) const {
//     DrawRectangle({button.x0y0_, button.x1y1_, button.color_});

//     if (!CmpDbl(button.buttonText_.position_.x0y0_.x_, -1) ||
//         !CmpDbl(button.buttonText_.position_.x0y0_.y_, -1)) {

//         DrawText(button.buttonText_);
//     }
// } 

// void Window::Resize(uint32_t width, uint32_t height) {
//     window_.setSize({width, height});
// }
