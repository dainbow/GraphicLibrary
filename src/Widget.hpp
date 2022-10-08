#pragma once

#include <chrono>

#include "Color.hpp"
#include "Event.hpp"
#include "Primitives.hpp"

class Window;

class Widget {
    protected:
        Widget* parent_;
        sf::RenderWindow* ptrToRealWdw_;

    public:
        int64_t width_;
        int64_t height_;

        uint32_t shiftX_;
        uint32_t shiftY_;

        Event<Vector> onMove_;
        Event<Vector> onClick_;
        Event<int64_t> onTick_;
        Event<int64_t> onRelease_;

        bool isClicked_;
        bool isHolded_;

        virtual Widget* GetParent() {
            return parent_;
        }

        virtual void SetParent(Widget* newParent) {
            parent_ = newParent;
        }

        virtual sf::RenderWindow* GetRealWindowPtr() {
            return ptrToRealWdw_;
        }

        virtual void SetRealWindowPtr(sf::RenderWindow* newPtr) {
            ptrToRealWdw_ = newPtr;
        }

        virtual bool IsClicked(const Vector& coords) = 0;

        virtual void FlagClicked(const Vector& vec) {
            if (IsClicked(vec)) {
                isClicked_ = 1;
            }
        }

        virtual void FlagHolded([[maybe_unused]] const Vector& vec) {
            if (isClicked_ && IsClicked(vec)) {
                isHolded_ = 1;
            }
        }

        virtual void FlagReleased([[maybe_unused]] const int64_t& time) {
            isClicked_ = 0;
            isHolded_  = 0;
        }

        virtual void Draw(const int64_t& time) = 0;

        Vector ConvertXY(int64_t x, int64_t y) {
            Widget* curWidg = this;

            while (curWidg->parent_) {
                x += curWidg->shiftX_;
                y += curWidg->shiftY_;

                curWidg = curWidg->parent_;
            }

            return {double(x), double(y)};
        }

        Vector ConvertRealXY(int64_t x, int64_t y) {
            Widget* curWidg = this;

            while (curWidg->parent_) {
                x -= curWidg->shiftX_;
                y -= curWidg->shiftY_;

                curWidg = curWidg->parent_;
            }

            return {double(x), double(y)};
        }

        Widget& operator=([[maybe_unused]] const Widget& widgToCpy) = default;
        Widget(const Widget& widgToCpy) = default;

        Widget(uint32_t shiftX, uint32_t shiftY, int64_t width, int64_t height) :
        parent_(nullptr), ptrToRealWdw_(nullptr),
        width_(width), height_(height),
        shiftX_(shiftX), shiftY_(shiftY),
        onMove_(), onClick_(), onTick_(), onRelease_(),
        isClicked_(0), isHolded_(0)
        {
            onClick_   += new MethodCaller<Widget, Vector>(this, &Widget::FlagClicked);
            onClick_   += new MethodCaller<Widget, Vector>(this, &Widget::FlagHolded);
            onRelease_ += new MethodCaller<Widget, int64_t>(this, &Widget::FlagReleased);

            onTick_    += new MethodCaller<Widget, int64_t>(this, &Widget::Draw);
        }

        virtual ~Widget() {}
};

class ChildrenManager {
    private:
        std::list<Widget*> widgets_;

    public:
        ChildrenManager() :
        widgets_()
        {}

        std::list<Widget*>* GetWidgetsList() {
            return &widgets_;
        }

        void TriggerClick(const Vector& coords) {
            for (auto& curWidget : widgets_) {
                curWidget->onClick_(coords);
            }
        }

        void TriggerMove(const Vector& coords) {
            for (auto& curWidget : widgets_) {
                curWidget->onMove_(coords);
            }
        }

        void TriggerTick(const int64_t& time) {
            for (auto& curWidget : widgets_) {
                curWidget->onTick_(time);
            }
        }

        void TriggerRelease(const int64_t& time) {
            for (auto& curWidget : widgets_) {
                curWidget->onRelease_(time);
            }
        }

        void operator+=(Widget* newWidget) {
            widgets_.push_back(newWidget);
        }

        void operator-=(Widget* widgetToClose) {
            for (auto it = widgets_.begin(); it != widgets_.end(); it++) {
                if (*it == widgetToClose) {
                    widgets_.erase(it);
                }
            }  
        }

        ~ChildrenManager() {
            for (auto& curWidget : widgets_) {
                delete curWidget;
            }
        }
};
