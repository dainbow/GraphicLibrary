#pragma once

#include "Widget.hpp"
#include "Utilities.hpp"

class DynamicWindow;

class Window : public Widget {
    protected:
        ChildrenManager manager_;
    public:
        Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Widget(x, y, width, height),
        manager_()
        {
            widgetSkin_ = SkinIdxs::WindowBackground;
        }

        ~Window() {}

        void Clear() {
            manager_.Clear();
        }

        virtual void OnMove(const Event& curEvent) override {
            Widget::OnMove(curEvent);

            manager_.TriggerMove(curEvent);
        }

        virtual bool IsClicked(const CordsPair& vec) override {
            CordsPair realVec = ConvertRealXY(vec);

            return ((realVec.x >= 0)  && (realVec.x <= width_) && 
                    (realVec.y >= 0)  && (realVec.y <= height_));
        }

        virtual void OnClick(const Event& curEvent) override {
            Widget::OnClick(curEvent);

            manager_.TriggerClick(curEvent);
        }

        virtual void Draw() override {
            CordsPair newXY = ConvertXY({0, 0});

            Rectangle realWindowRect({width_, height_, int64_t(newXY.x), int64_t(newXY.y), GetRotation()});
            realWindowRect.Draw(ptrToRealWdw_, skinManager_->GetTexture(widgetSkin_));  
        }

        virtual void OnTick(const Event& curEvent) override {
            Widget::OnTick(curEvent);

            manager_.TriggerTick(curEvent);
        }

        virtual void OnRelease(const Event& curEvent) override {
            Widget::OnRelease(curEvent);

            manager_.TriggerRelease(curEvent);
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            manager_.TriggetKeyPressed(curEvent);
        }

        virtual void operator+=(Widget* newWidget) {
            newWidget->SetParent(this);
            newWidget->SetRealWindowPtr(ptrToRealWdw_);
            newWidget->SetSkinManager(skinManager_);

            manager_ += newWidget;
        }

        void operator-=(Widget* widgetToDisconnect) {
            manager_ -= widgetToDisconnect;

            widgetToDisconnect->SetSkinManager(nullptr);
            widgetToDisconnect->SetRealWindowPtr(nullptr);
            widgetToDisconnect->SetParent(nullptr);
        }


        virtual void SetParent(Widget* newParent) override {
            parent_ = newParent;

            for (auto& curChild : *manager_.GetWidgetsList()) {
                curChild->SetParent(this);
            }
        }

        virtual void SetRealWindowPtr(sf::RenderWindow* newPtr) override {
            ptrToRealWdw_ = newPtr;

            for (auto& curChild : *manager_.GetWidgetsList()) {
                curChild->SetRealWindowPtr(newPtr);
            }
        }

        virtual void SetSkinManager(SkinManager* skinManager) override {
            skinManager_ = skinManager;

            for (auto& curChild : *manager_.GetWidgetsList()) {
                curChild->SetSkinManager(skinManager);
            }
        }
};

const int64_t TimeBetweenClicks = 30;
const int64_t TimeBetweenKeys   = 50;
const int64_t TimeBetweenTicks  = 10;

class RealWindow final : public Window {
    private:
        sf::RenderWindow realWindow_;

        int64_t lastPressedTime_;
        int64_t lastKeyPressedTime_;
        int64_t lastReleasedTime_;
        int64_t lastTickTime_;
        int64_t lastMoveTime_;
    public:
        RealWindow::RealWindow(uint32_t width, uint32_t height, SkinManager* skinManager) :
        Window(0, 0, width, height),
        realWindow_(sf::VideoMode(width, height), "Window"),
        lastPressedTime_(0), lastKeyPressedTime_(0), lastReleasedTime_(0), lastTickTime_(0), lastMoveTime_(0)
        {   
            widgetSkin_ = SkinIdxs::RealWindowBackground;

            ptrToRealWdw_ = &realWindow_;
            skinManager_ = skinManager;
        };

        ~RealWindow() {
            if (realWindow_.isOpen()) {
                realWindow_.close();
            }
        }

        virtual bool IsClicked([[maybe_unused]] const CordsPair& vec) override {
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

        void PollEvent() {
            sf::Event curEvent;
            realWindow_.pollEvent(curEvent);

            switch (curEvent.type) {
                case sf::Event::Closed: {
                    Close();

                    break;
                }

                case sf::Event::MouseMoved: {
                    if ((GetTimeMiliseconds() - lastMoveTime_) < TimeBetweenTicks) 
                        break;

                    if ((curEvent.mouseMove.y < 0) || (curEvent.mouseButton.y > height_)) {
                        break;
                    }

                    OnMove(curEvent);

                    lastMoveTime_ = GetTimeMiliseconds();
                    break;
                }

                case sf::Event::MouseButtonPressed: {
                    if ((GetTimeMiliseconds() - lastPressedTime_) < TimeBetweenClicks)
                        break;

                    OnClick(curEvent);

                    lastPressedTime_ = GetTimeMiliseconds();
                    break;
                }

                case sf::Event::MouseButtonReleased: {
                    if ((GetTimeMiliseconds() - lastReleasedTime_) < TimeBetweenKeys)
                        break;

                    OnRelease(curEvent);

                    lastReleasedTime_ = GetTimeMiliseconds();
                    break;
                }

                case sf::Event::KeyPressed: {
                    if ((GetTimeMiliseconds() - lastKeyPressedTime_) < TimeBetweenKeys)
                        break;

                    std::cout << uint32_t(curEvent.key.code) << std::endl;

                    OnKeyboard(curEvent);

                    lastKeyPressedTime_ = GetTimeMiliseconds();
                    break;
                }

                case sf::Event::Count:
                case sf::Event::Resized:
                case sf::Event::LostFocus:
                case sf::Event::GainedFocus:
                case sf::Event::TextEntered:
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

            if ((GetTimeMiliseconds() - lastTickTime_) < TimeBetweenTicks)
                return;

            OnTick(curEvent);
            lastTickTime_ = GetTimeMiliseconds();
        }
};

class DynamicWindow : public Window {
    public:
        DynamicWindow(uint32_t x, uint32_t y) :
        Window(x, y, 0, 0)
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
    public:
        double curPointer_;

        double minHeight_;
        double maxHeight_;

        LimitedWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Window(x, y, width, height),
        curPointer_(0),
        minHeight_(0), maxHeight_(0)
        {}

        virtual void operator+=(Widget* newWidget) override {
            maxHeight_ += double(newWidget->height_);

            Window::operator+=(newWidget);
        }

        virtual void OnMove(const Event& curEvent) override {
            MoveVisible(curEvent);
        }

        virtual void OnClick(const Event& curEvent) override {
            ClickVisible(curEvent);
        }

        virtual void OnTick(const Event& curEvent) override {
            Draw();
            TickVisible(curEvent);
        }

        virtual void OnRelease(const Event& curEvent) override {
            ReleaseVisible(curEvent);
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            KeyVisible(curEvent);
        }

    private:
        void TriggerVisible(void (Widget::* method)(const Event& curEvent), const Event& curEvent) {
            uint32_t curHeight = 0;
            uint32_t curShownHeight = 0;

            for (auto& curWidget : *manager_.GetWidgetsList()) {
                if ((curHeight >= uint32_t(curPointer_)) && ((curShownHeight + curWidget->height_) <= height_) && ((curWidget->width_ + curWidget->shiftX_) <= width_)) {
                    curWidget->shiftY_ = curShownHeight;
                    (curWidget->*method)(curEvent);

                    curShownHeight += uint32_t(curWidget->height_);
                }

                curHeight += uint32_t(curWidget->height_);
            }
        }

        void MoveVisible(const Event& curEvent) {
            TriggerVisible(&Widget::OnMove, curEvent);
        }

        void ClickVisible(const Event& curEvent) {
            TriggerVisible(&Widget::OnClick, curEvent);
        }

        void TickVisible(const Event& curEvent) {
            TriggerVisible(&Widget::OnTick, curEvent);
        }

        void ReleaseVisible(const Event& curEvent) {
            TriggerVisible(&Widget::OnRelease, curEvent);
        }

        void KeyVisible(const Event& curEvent) {
            TriggerVisible(&Widget::OnKeyboard, curEvent);
        }
};

