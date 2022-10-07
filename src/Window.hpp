#pragma once

#include "Widget.hpp"

class DynamicWindow;

class Window : public Widget {
        friend Widget;
        friend DynamicWindow;

    protected:
        ChildrenManager manager_;

        int64_t width_;
        int64_t height_;

        MyColor backColor_;

        Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const MyColor& color) :
        Widget(x, y),
        manager_(),
        width_(width), height_(height),
        backColor_(color)
        {
            onTick_  += new MethodCaller<ChildrenManager, int64_t>(&manager_, &ChildrenManager::TriggerTick);
            onClick_ += new MethodCaller<ChildrenManager, Vector>(&manager_, &ChildrenManager::TriggerClick);
            onMove_  += new MethodCaller<ChildrenManager, Vector>(&manager_, &ChildrenManager::TriggerMove);
            onRelease_ += new MethodCaller<ChildrenManager, int64_t>(&manager_, &ChildrenManager::TriggerRelease);
        }

    public:
        virtual void Draw([[maybe_unused]] const int64_t& time) override {
            Vector newXY = ConvertXY(0, 0);

            Rectangle realWindowRect({width_, height_, int64_t(newXY.x_), int64_t(newXY.y_)});

            realWindowRect.Draw(ptrToRealWdw_, backColor_);  
        }

        void operator+=(Widget* newWidget) {
            newWidget->SetParent(this);
            newWidget->SetRealWindowPtr(ptrToRealWdw_);

            manager_ += newWidget;
        }

        void operator-=(Widget* widgetToDisconnect) {
            manager_ -= widgetToDisconnect;

            widgetToDisconnect->SetParent(nullptr);
            widgetToDisconnect->SetRealWindowPtr(nullptr);
        }

        virtual bool IsClicked(const Vector& vec) override {
            Vector x0y0 = ConvertXY(0, 0);
            Vector x1y1 = ConvertXY(width_, height_);

            return ((vec.x_ >= x0y0.x_)  && (vec.x_ <= x1y1.x_) && 
                    (vec.y_ >= x0y0.y_)  && (vec.y_ <= x1y1.y_));
        }

        ~Window() {

        }
};

class RealWindow final : public Window {
    private:
        sf::RenderWindow realWindow_;

        bool isPressed_;
    public:
        RealWindow(uint32_t width, uint32_t height, const MyColor& color);

        void PollEvent() {
            sf::Event curEvent;
            realWindow_.pollEvent(curEvent);

            switch (curEvent.type) {
                case sf::Event::Closed: {
                    Close();

                    break;
                }

                case sf::Event::MouseMoved: {
                    // Vector moveCoords = {double(curEvent.mouseMove.x), double(curEvent.mouseMove.y)};

                    // onMove_(moveCoords);

                    break;
                }

                case sf::Event::MouseButtonPressed: {
                    if (isPressed_)
                        break;

                    Vector clickCoords = {double(curEvent.mouseButton.x), double(curEvent.mouseButton.y)};

                    onClick_(clickCoords);

                    isPressed_ = 1;
                    break;
                }

                case sf::Event::MouseButtonReleased: {
                    onRelease_(GetTimeMiliseconds());

                    isPressed_ = 0;
                    break;
                }

                case sf::Event::Count:
                case sf::Event::Resized:
                case sf::Event::LostFocus:
                case sf::Event::GainedFocus:
                case sf::Event::TextEntered:
                case sf::Event::KeyPressed:
                case sf::Event::KeyReleased:
                case sf::Event::MouseWheelMoved:
                case sf::Event::MouseWheelScrolled:
                case sf::Event::MouseEntered:
                case sf::Event::MouseLeft:
                case sf::Event::JoystickButtonPressed:
                case sf::Event::JoystickButtonReleased:
                case sf::Event::JoystickConnected:
                case sf::Event::JoystickDisconnected:
                case sf::Event::JoystickMoved:
                case sf::Event::TouchBegan:
                case sf::Event::TouchMoved:
                case sf::Event::TouchEnded:
                case sf::Event::SensorChanged:
                default:
                    break;
            }

            onTick_(GetTimeMiliseconds());
        }

        virtual bool IsClicked([[maybe_unused]] const Vector& vec) override {
            return true;
        }

        bool IsOpen() {
            return realWindow_.isOpen();
        }

        void Close() {
            realWindow_.close();
        }

        void Clear() {
            realWindow_.clear();
        }

        void Display() {
            realWindow_.display();
        }

        ~RealWindow() {
            if (realWindow_.isOpen()) {
                realWindow_.close();
            }
        }
};

class DynamicWindow : public Window {
    protected:

    public:
        DynamicWindow(uint32_t x, uint32_t y, const MyColor& color) :
        Window(x, y, 0, 0, color)
        {}

        void operator+=(Window* windowToAdd) {
            windowToAdd->shiftY_ = uint32_t(height_);
            height_ += windowToAdd->height_;

            windowToAdd->shiftX_ = 0;
            width_ = std::max(width_, windowToAdd->width_);


            windowToAdd->SetParent(this);
            windowToAdd->SetRealWindowPtr(ptrToRealWdw_);
            manager_ += windowToAdd;
        }
};

