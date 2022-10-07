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
        uint32_t shiftX_;
        uint32_t shiftY_;

        Event<Vector> onMove_;
        Event<Vector> onClick_;
        Event<int64_t> onTick_;
        Event<int64_t> onRelease_;

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

        virtual void Draw(const int64_t& time) = 0;

        Vector ConvertXY(int64_t x, int64_t y) {
            Widget* curWidg = this;;

            while (curWidg->parent_) {
                x += curWidg->shiftX_;
                y += curWidg->shiftY_;

                curWidg = curWidg->parent_;
            }

            return {double(x), double(y)};
        }

        Widget& operator=([[maybe_unused]] const Widget& widgToCpy) {
            return *this;
        }

        Widget(const Widget& widgToCpy) :
        parent_(nullptr), ptrToRealWdw_(nullptr),
        shiftX_(widgToCpy.shiftX_), shiftY_(widgToCpy.shiftY_),
        onMove_(widgToCpy.onMove_), onClick_(widgToCpy.onClick_), onTick_(widgToCpy.onTick_), onRelease_(widgToCpy.onRelease_)
        {}

        Widget(uint32_t shiftX, uint32_t shiftY) :
        parent_(nullptr), ptrToRealWdw_(nullptr),
        shiftX_(shiftX), shiftY_(shiftY),
        onMove_(), onClick_(), onTick_(), onRelease_()
        {
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
