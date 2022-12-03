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

            return ((realVec.x >= 0)  && (realVec.x <= GetWidth()) && 
                    (realVec.y >= 0)  && (realVec.y <= GetHeight()));
        }

        virtual void OnClick(const Event& curEvent) override {
            Widget::OnClick(curEvent);

            manager_.TriggerClick(curEvent);
        }

        virtual void ReDraw() override {
            Rectangle realWindowRect({GetWidth(), GetHeight(), 0, 0, 0});
            realWindowRect.Draw(widgetContainer_, SkinManager::GetInstance("default").GetTexture(widgetSkin_));  
        }

        virtual void OnTick(const Event& curEvent) override {
            ProcessRedraw();
            manager_.TriggerTick(curEvent);

            PlaceTexture();
        }

        virtual void OnRelease(const Event& curEvent) override {
            Widget::OnRelease(curEvent);

            manager_.TriggerRelease(curEvent);
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            manager_.TriggetKeyPressed(curEvent);
        }

        virtual void OnKeyboardRelease(const Event& curEvent) override {
            manager_.TriggerKeyboardRelease(curEvent);
        }

        virtual void operator+=(Widget* newWidget) {
            newWidget->SetParent(this);

            manager_ += newWidget;
        }

        void operator-=(Widget* widgetToDisconnect) {
            manager_ -= widgetToDisconnect;

            widgetToDisconnect->SetParent(nullptr);
        }


        virtual void SetParent(Widget* newParent) override {
            parent_ = newParent;

            for (auto& curChild : *manager_.GetWidgetsList()) {
                curChild->SetParent(this);
            }
        }
};

const int64_t TimeBetweenClicks = 30;
const int64_t TimeBetweenKeys   = 50;
const int64_t TimeBetweenTicks  = 10;

class RealWindow : public Window {
    protected:
        sf::RenderWindow realWindow_;

        int64_t lastPressedTime_;
        int64_t lastKeyPressedTime_;
        int64_t lastReleasedTime_;
        int64_t lastTickTime_;
        int64_t lastMoveTime_;
    public:
        RealWindow(uint32_t width, uint32_t height) :
        Window(0, 0, width, height),
        realWindow_(sf::VideoMode(width, height), "Window"),
        lastPressedTime_(0), lastKeyPressedTime_(0), lastReleasedTime_(0), lastTickTime_(0), lastMoveTime_(0)
        {   
            widgetSkin_ = SkinIdxs::RealWindowBackground;
        };

        ~RealWindow() {
            if (realWindow_.isOpen()) {
                realWindow_.close();
            }
        }

        virtual void PlaceTexture() override {
            sf::Sprite sprite(widgetContainer_.getTexture());
            sprite.setPosition({float(GetShiftX()), float(GetShiftY())});

            realWindow_.draw(sprite);
        }

        virtual bool IsClicked([[maybe_unused]] const CordsPair& vec) override {
            return true;
        }

        void Hide() {
            realWindow_.setVisible(0);
        }

        void Show() {
            realWindow_.setVisible(1);
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

        virtual void PollEvent() {
            sf::Event curEvent;
            if (realWindow_.pollEvent(curEvent) == 0) {
                OnTick(Event(curEvent));

                return;
            }

            Event myEvent(curEvent);

            switch (myEvent.type_) {
                case EventType::Closed: {
                    Close();

                    break;
                }

                case EventType::MouseMoved: {
                    // if ((GetTimeMiliseconds() - lastMoveTime_) < TimeBetweenTicks) 
                    //     break;

                    if ((curEvent.mouseMove.y < 0) || (curEvent.mouseMove.y > GetHeight())) {
                        break;
                    }

                    OnMove(myEvent);

                    // lastMoveTime_ = GetTimeMiliseconds();
                    break;
                }

                case EventType::MousePressed: {
                    if ((GetTimeMiliseconds() - lastPressedTime_) < TimeBetweenClicks)
                        break;

                    OnClick(myEvent);

                    lastPressedTime_ = GetTimeMiliseconds();
                    break;
                }

                case EventType::MouseReleased: {
                    // if ((GetTimeMiliseconds() - lastReleasedTime_) < TimeBetweenKeys)
                    //     break;

                    OnRelease(myEvent);

                    // lastReleasedTime_ = GetTimeMiliseconds();
                    break;
                }

                case EventType::KeyPressed: {
                    // if ((GetTimeMiliseconds() - lastKeyPressedTime_) < TimeBetweenKeys)
                    //     break;

                    // std::cout << uint32_t(curEvent.key.code) << std::endl;

                    OnKeyboard(myEvent);

                    // lastKeyPressedTime_ = GetTimeMiliseconds();
                    break;
                }
                
                case EventType::KeyReleased: {

                }

                case EventType::NoEvent:
                case EventType::ButtonClicked:
                case EventType::ScrollbarMoved:
                case EventType::CanvasMPressed:
                case EventType::CanvasMReleased:
                case EventType::CanvasMMoved:
                case EventType::CanvasMLeft:
                case EventType::TimerEvent:
                default:
                    break;
            }

            OnTick(myEvent);

            // if ((GetTimeMiliseconds() - lastTickTime_) < TimeBetweenTicks)
            //     return;
            // lastTickTime_ = GetTimeMiliseconds();
        }
};

class App {
    private: 
        static App* instance_;
        std::vector<RealWindow*> windows_;

        App() : windows_() {}
    public:
        static App& GetInstance() {
            if (!instance_) {
                instance_ = new App();
            }

            return *instance_;
        }

        ~App() {
            for (auto curWindow : windows_) {
                delete curWindow;
            }
        }

        void operator+=(RealWindow* newWindow) {
            windows_.push_back(newWindow);
        }

        bool IsOpen() {
            bool flag = 0;
            for (auto curWindow : windows_) {
                flag |= curWindow->IsOpen();
            }

            return flag;
        }

        void PollEvent() {
            for (auto curWindow : windows_) {
                curWindow->PollEvent();
            }
        }

        void Clear() {
            for (auto curWindow : windows_) {
                curWindow->Clear();
            }
        }

        void Display() {
            for (auto curWindow : windows_) {
                curWindow->Display();
            }
        }

};

class DynamicWindow : public Window {
    public:
        DynamicWindow(uint32_t x, uint32_t y) :
        Window(x, y, 0, 0)
        {}

        void operator+=(Widget* windowToAdd) override {
            windowToAdd->SetShifts(0, uint32_t(GetHeight()));
            SetSizes(std::max(GetWidth(), windowToAdd->GetWidth()), GetHeight() + windowToAdd->GetHeight());

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
            maxHeight_ += double(newWidget->GetHeight());

            Window::operator+=(newWidget);
        }

        virtual void OnMove(const Event& curEvent) override {
            MoveVisible(curEvent);
        }

        virtual void OnClick(const Event& curEvent) override {
            ClickVisible(curEvent);
        }

        virtual void OnTick(const Event& curEvent) override {
            ProcessRedraw();
            TickVisible(curEvent);

            PlaceTexture();
        }

        virtual void OnRelease(const Event& curEvent) override {
            ReleaseVisible(curEvent);
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            KeyVisible(curEvent);
        }

    protected:
        void TriggerVisible(void (Widget::* method)(const Event& curEvent), const Event& curEvent) {
            uint32_t curHeight = 0;
            uint32_t curShownHeight = 0;

            for (auto& curWidget : *manager_.GetWidgetsList()) {
                if ((curHeight >= uint32_t(curPointer_)) && ((curShownHeight + curWidget->GetHeight()) <= GetHeight()) && ((curWidget->GetWidth() + curWidget->GetShiftX()) <= GetWidth())) {
                    curWidget->SetShifts(curWidget->GetShiftX(), curShownHeight);
                    (curWidget->*method)(curEvent);

                    curShownHeight += uint32_t(curWidget->GetHeight());
                }

                curHeight += uint32_t(curWidget->GetHeight());
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

