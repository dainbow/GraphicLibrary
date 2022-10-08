#pragma once

#include "Widget.hpp"

class DynamicWindow;

class Window : public Widget {
        friend Widget;
        friend DynamicWindow;

    protected:
        ChildrenManager manager_;

        MyColor backColor_;
    public:
        Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const MyColor& color) :
        Widget(x, y, width, height),
        manager_(),
        backColor_(color)
        {
            onTick_  += new MethodCaller<ChildrenManager, int64_t>(&manager_, &ChildrenManager::TriggerTick);
            onClick_ += new MethodCaller<ChildrenManager, Vector>(&manager_, &ChildrenManager::TriggerClick);
            onMove_  += new MethodCaller<ChildrenManager, Vector>(&manager_, &ChildrenManager::TriggerMove);
            onRelease_ += new MethodCaller<ChildrenManager, int64_t>(&manager_, &ChildrenManager::TriggerRelease);
        }

        virtual void Draw([[maybe_unused]] const int64_t& time) override {
            Vector newXY = ConvertXY(0, 0);

            Rectangle realWindowRect({width_, height_, int64_t(newXY.x_), int64_t(newXY.y_)});

            realWindowRect.Draw(ptrToRealWdw_, backColor_);  
        }

        virtual void operator+=(Widget* newWidget) {
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

const int64_t TimeBetweenClicks = 30;

class RealWindow final : public Window {
    private:
        sf::RenderWindow realWindow_;

        int64_t lastPressedTime_;
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
                    Vector moveCoords = {double(curEvent.mouseMove.x), double(curEvent.mouseMove.y)};

                    onMove_(moveCoords);

                    break;
                }

                case sf::Event::MouseButtonPressed: {
                    if ((GetTimeMiliseconds() - lastPressedTime_) < TimeBetweenClicks)
                        break;

                    Vector clickCoords = {double(curEvent.mouseButton.x), double(curEvent.mouseButton.y)};
                    onClick_(clickCoords);

                    lastPressedTime_ = GetTimeMiliseconds();
                    break;
                }

                case sf::Event::MouseButtonReleased: {
                    onRelease_(GetTimeMiliseconds());

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

        void operator+=(Widget* windowToAdd) override {
            windowToAdd->shiftY_ = uint32_t(height_);
            height_ += windowToAdd->height_;

            windowToAdd->shiftX_ = 0;
            width_ = std::max(width_, windowToAdd->width_);

            Window::operator+=(windowToAdd);
        }
};

class LimitedWindow : public Window {
    private:
        void ClickVisible(ChildrenManager* obj, const Vector& vec) {
            uint32_t curHeight      = 0;
            uint32_t curShownHeight = 0;

            for (auto& curWidget : *obj->GetWidgetsList()) {
                if ((curHeight >= uint32_t(curPointer_)) && ((curShownHeight + curWidget->height_) <= height_) && ((curWidget->width_ + curWidget->shiftX_) <= width_)) {
                    curWidget->shiftY_ = curShownHeight;
                    curWidget->onClick_(vec);

                    curShownHeight += uint32_t(curWidget->height_);
                }

                curHeight += uint32_t(curWidget->height_);
            }
        }

        void TickVisible(ChildrenManager* obj, const int64_t& time) {
            uint32_t curHeight      = 0;
            uint32_t curShownHeight = 0;

            for (auto& curWidget : *obj->GetWidgetsList()) {
                if ((curHeight >= uint32_t(curPointer_)) && ((curShownHeight + curWidget->height_) <= height_) && ((curWidget->width_ + curWidget->shiftX_) <= width_)) {
                    curWidget->shiftY_ = curShownHeight;
                    curWidget->onTick_(time);

                    curShownHeight += uint32_t(curWidget->height_);
                }

                curHeight += uint32_t(curWidget->height_);
            }
        }

        void MoveVisible(ChildrenManager* obj, const Vector& vec) {
            uint32_t curHeight = 0;
            uint32_t curShownHeight = 0;

            for (auto& curWidget : *obj->GetWidgetsList()) {
                if ((curHeight >= uint32_t(curPointer_)) && ((curShownHeight + curWidget->height_) <= height_) && ((curWidget->width_ + curWidget->shiftX_) <= width_)) {
                    curWidget->shiftY_ = curShownHeight;
                    curWidget->onMove_(vec);

                    curShownHeight += uint32_t(curWidget->height_);
                }

                curHeight += uint32_t(curWidget->height_);
            }
        }

        void ReleaseVisible(ChildrenManager* obj, const int64_t& time) {
            uint32_t curHeight = 0;
            uint32_t curShownHeight = 0;

            for (auto& curWidget : *obj->GetWidgetsList()) {
                if ((curHeight >= uint32_t(curPointer_)) && ((curShownHeight + curWidget->height_) <= height_) && ((curWidget->width_ + curWidget->shiftX_) <= width_)) {
                    curWidget->shiftY_ = curShownHeight;
                    curWidget->onRelease_(time);

                    curShownHeight += uint32_t(curWidget->height_);
                }

                curHeight += uint32_t(curWidget->height_);
            }
        }

    public:
        double curPointer_;

        double minHeight_;
        double maxHeight_;

        virtual void operator+=(Widget* newWidget) override {
            maxHeight_ += double(newWidget->height_);

            Window::operator+=(newWidget);
        }

        LimitedWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const MyColor& color) :
        Window(x, y, width, height, color),
        curPointer_(0),
        minHeight_(0), maxHeight_(0)
        {
            onTick_.Clear();
            onClick_.Clear();
            onMove_.Clear();
            onRelease_.Clear();

            onTick_    += new MethodCaller<Widget, int64_t>(this, &Widget::Draw);
            onTick_    += new AddMethodCaller<LimitedWindow, ChildrenManager, int64_t>(this, &manager_, &LimitedWindow::TickVisible);
            onClick_   += new AddMethodCaller<LimitedWindow, ChildrenManager, Vector>(this, &manager_, &LimitedWindow::ClickVisible);
            onMove_    += new AddMethodCaller<LimitedWindow, ChildrenManager, Vector>(this, &manager_, &LimitedWindow::MoveVisible);
            onRelease_ += new AddMethodCaller<LimitedWindow, ChildrenManager, int64_t>(this, &manager_, &LimitedWindow::ReleaseVisible);
        }
};

